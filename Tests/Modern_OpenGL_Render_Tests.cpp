#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <windows.h>
void Check(bool value, const char *message) {
  if (!value)
    throw std::runtime_error(message);
}
template <class T> T Fn(HMODULE plugin, const char *name) {
  auto address = GetProcAddress(plugin, name);
  Check(address, name);
  return reinterpret_cast<T>(address);
}
struct Library {
  HMODULE Plugin = nullptr;
  ~Library() {
    if (Plugin) {
      try {
        Fn<void (*)()>(Plugin, "Close_All")();
        Fn<void (*)()>(Plugin, "Destructor")();
      } catch (...) {
      }
      FreeLibrary(Plugin);
    }
  }
};
std::vector<unsigned char> Read() {
  std::vector<unsigned char> pixels(640 * 480 * 3);
  glFinish();
  glReadBuffer(GL_FRONT);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, 640, 480, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
  Check(glGetError() == GL_NO_ERROR, "core GL error");
  return pixels;
}
bool Magenta(const std::vector<unsigned char> &p, int x, int y) {
  size_t i = (y * 640 + x) * 3;
  return p[i] > 240 && p[i + 1] < 10 && p[i + 2] > 240;
}
int main(int argc, char **argv) {
  try {
    Check(argc > 1, "resources path required");
    Library library;
    auto &p = library.Plugin;
    p = LoadLibraryA("Resources/Modules/Appeal_Module_Graphics/"
                     "Appeal_SDL2_Modern_OpenGL_Graphics/"
                     "Appeal_SDL2_Modern_OpenGL_Graphics.dll");
    Check(p, "load modern DLL");
    auto add =
        Fn<void (*)(std::string, std::string, int, int)>(p, "Add_Window");
    auto render = Fn<void (*)()>(p, "Render");
    auto close = Fn<void (*)()>(p, "Close_All");
    auto location =
        Fn<void (*)(std::string, float, float, float)>(p, "Set_Location");
    auto rotate =
        Fn<void (*)(std::string, float, float, float)>(p, "Set_Orientation");
    auto shader = Fn<void (*)(std::string, std::string)>(p, "Set_Shader");
    auto loadShader = Fn<void (*)(std::string, std::string)>(p, "Load_Shader");
    Fn<void (*)(std::string)>(p, "Set_Paths")(argv[1]);
    add("Main", "Modern OpenGL render validation", 640, 480);
    GLint major = 0, minor = 0, profile = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    Check((major > 3 || (major == 3 && minor >= 3)) &&
              (profile & GL_CONTEXT_CORE_PROFILE_BIT),
          "OpenGL 3.3 core required");
    Fn<void (*)(bool)>(p, "Set_GUI_Visible")(false);
    Fn<void (*)(std::string, std::string)>(p, "Load_Mesh")("Triangle",
                                                           "Triangle.obj");
    Fn<void (*)(std::string, std::string)>(p, "Load_Asset")("Triangle",
                                                            "Triangle");
    render();
    auto original = Read();
    Check(original[(240 * 640 + 320) * 3 + 1] > 50, "lit mesh must render");
    location("Triangle", 1, 0, 0);
    render();
    auto moved = Read();
    Check(moved != original, "3D translation");
    rotate("Triangle", 0, 0, 90);
    render();
    Check(Read() != moved, "3D rotation");
    location("Triangle", 0, 0, 0);
    rotate("Triangle", 0, 0, 0);
    loadShader("Default", "Default.shader");
    shader("Triangle", "Default");
    render();
    Check(Read() == original, "file shader matches embedded shader");
    loadShader("Tint", "Tint.shader");
    shader("Triangle", "Tint");
    render();
    Check(Magenta(Read(), 320, 240), "custom fragment shader");
    // Shader compile and program-link diagnostics must be actionable, and
    // cleanup must recover.
    bool compileFailed = false, linkFailed = false;
    try {
      loadShader("Invalid", "Invalid.shader");
    } catch (const std::exception &e) {
      compileFailed =
          std::string(e.what()).find("compilation failed") != std::string::npos;
    }
    try {
      loadShader("Broken_Link", "Broken_Link.shader");
    } catch (const std::exception &e) {
      linkFailed =
          std::string(e.what()).find("link failed") != std::string::npos;
    }
    Check(compileFailed && linkFailed, "shader error logs");
    shader("Triangle", "Default");
    render();
    Check(Read() == original, "recover from invalid shaders");
    // Validate the archived project's GLSL 140 shaders/config without rewriting
    // them.
    loadShader("Reference", "Reference/Default.Config");
    const unsigned char pixels[] = {255, 0, 0, 255, 255, 0, 0, 255,
                                    255, 0, 0, 255, 255, 0, 0, 255};
    Check(Fn<bool (*)(const char *, int, int, int, const unsigned char *)>(
              p, "Video_Upload")("Video_Texture", 2, 2, 8, pixels),
          "upload video texture");
    Fn<void (*)(std::string, std::string)>(p, "Set_Texture")("Triangle",
                                                             "Video_Texture");
    shader("Triangle", "Reference");
    render();
    auto textured = Read();
    size_t c = (240 * 640 + 320) * 3;
    Check(textured[c] > 240 && textured[c + 1] < 10,
          "reference shader texture sample");
    Fn<void (*)(const char *, int, int, int, int)>(p, "Video_Draw")(
        "Video_Texture", 20, 20, 80, 60);
    Fn<void (*)(std::string, float, float, float, float, int, int, int, int)>(
        p, "Draw_Rect")("Overlay", 110, 20, 60, 60, 0, 255, 0, 255);
    render();
    auto video = Read();
    c = ((480 - 40) * 640 + 40) * 3;
    Check(video[c] > 240 && video[c + 1] < 10, "video sprite");
    c = ((480 - 40) * 640 + 130) * 3;
    Check(video[c] < 10 && video[c + 1] > 240,
          "overlay must use default shader");
    if (argc > 2) {
      std::ofstream image(argv[2], std::ios::binary);
      image << "P6\n640 480\n255\n";
      for (int y = 479; y >= 0; --y)
        image.write(reinterpret_cast<char *>(video.data() + y * 640 * 3),
                    640 * 3);
    }
    // SDL shares textures/programs; each context must still own its own VAO.
    add("Second", "Second core context", 640, 480);
    render();
    Read();
    Fn<void (*)(std::string)>(p, "Delete_Window")("Main");
    render();
    Read();
    close();
    add("Reopened", "Recreated core context", 640, 480);
    render();
    Read();
    close();
    std::cout << "MODERN_OPENGL_RENDER_PASSED\n";
    return 0;
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
