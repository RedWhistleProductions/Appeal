# Five Questions

A complete small single-player quiz written in Appeal, using existing modules,
plugins and reusable libraries. From the Appeal checkout, run:

```powershell
.\Appeal.exe Games/Five_Questions/Main
```

Choose **New game**, answer five questions, and get at least three correct to
pass. Each question has 30 seconds of gameplay time. After an answer or timeout,
the game shows feedback and the correct answer; choose Next to continue. Results
offer Play again, Back to title, or Quit. Both passing and failing runs are valid
game outcomes, and restarting resets the score and question sequence.

Use the mouse or native GUI keyboard navigation (Tab/arrows, then Enter).
Controller navigation is supplied by the GUI backend; physical controller use
has not been verified for this game. Alt+Enter uses the existing fullscreen toggle.

## Progress and settings

- Pause freezes the question clock. Resume continues it.
- Progress saves automatically at the start, after answers/advancement, on pause/
  resume, and when returning to title or quitting an active game.
- Continue restores a question, its feedback, a paused run, or a completed result.
- Best score and the Options sound setting persist independently of the run.
- The default database is `Resources/Storage/Five_Questions.appealdb`, following
  the existing Storage plugin configuration. It is separate from editor data.
  Choose a writable per-user/profile database path for another host/install setup.

Normal quit and the native window close path save the latest progress. Forced
termination/power loss can lose time since the latest save; no crash recovery or
corruption repair is promised. Save/load follows Game/Saves' existing limitations.

## Script organization

- Main.Appeal loads existing Graphics/Input/World/Storage/Audio plugins and owns
  the window/update loop. Production C++ is unchanged.
- Core.Appeal owns quiz rules, deadlines, score flow, save hooks and event handling.
- Questions.Appeal supplies question text, three answers, and the correct answer.
- Interface.Appeal draws title/questions/feedback/pause/options/results and queues
  commands through Game/Controls. Every answer/Next command carries its question
  number; handlers recheck it to reject stale/double clicks.
- Game/Runtime, Scenes, Saves, Rules, Interactions, Controls, Sound and Rounds
  supply reusable policies. World holds the saveable session scene.
- Correct/Wrong WAV files under Resources/Audio/SFX/Quiz are short original
  synthesized tones. No downloaded or licensed external artwork/music is needed.

Question count/pass target and display totals are deliberately fixed at five/
three in this MVP. If replacing the content with a different-sized quiz, update
Core and interface totals together. A large data-driven question bank is future
content/tooling work, not a requirement for this small game.

## Genre acceptance evidence

This demonstrates **basic single-player trivia/quiz readiness**: start, repeatable
question/answer loop, timed decisions, feedback, scoring, win/loss, pause, restart,
settings, save/load across launches, and quit. It does not claim all trivia variants,
multiplayer, physical controller support, or production distribution are complete.

Validation:

```powershell
python Tests/Five_Questions_Tests.py
python Tests/Five_Questions_Render_Tests.py
```

The first test uses real plugins and separate processes for saved-game continuation.
The second actually plays through the rendered GUI: keyboard start, mouse answers,
pause/title/continue/resume, a winning run, replay, a losing run, options and quit.
It verifies persisted results/settings and captures a result-screen image under
Tests/Artifacts. Audio testing uses SDL's dummy output rather than claiming a
human listening check. C++ files under Tests are automation harnesses, not game code.

The native callback/payload, input-context, audio-lifetime and serialization
limitations found along the way remain in Docs/CPP_Recommendations.md.
