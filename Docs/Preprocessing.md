# Script preprocessing and returned values

The runtime now reads through this pipeline:

```text
File / TCP adapter / Appeal function
                |
       Data_Source_Manager       selects the current raw source
                |
       Data_Source_Preprocessor  resolves variables and expressions
                |
            Interpreter          executes commands and reads arguments
```

DSM still holds the raw source list and variables. It no longer owns or calls
the preprocessor. Each interpreter invocation creates a preprocessor that reads
from DSM. Modules receive that preprocessor through the existing `Data_Source*`
argument, so their argument reads follow the same pipeline as built-in commands.

## Expressions

```text
Print(Input Mouse_X)
```

`Input` is the module name; `Mouse_X` is its existing getter keyword. Load and
initialize the Input module/plugin before using it. Spaces next to parentheses
are optional. Nested expressions work, and returned properties are supplied in
FIFO order to the arguments of the surrounding command.

When an argument starts with `(`, the preprocessor collects the balanced script
up to its matching `)`. It asks Appeal to execute that script synchronously with
a temporary source, a separate preprocessor, and a private result queue. The same
command dispatcher, modules, and variables are reused. Nested argument expressions
repeat this process. When execution returns, the waiting preprocessor inserts the
returned values. The caller's source and input buffers remain separate.

Completion is the return from expression execution, not an empty queue or a
`Return` value. An expression that finishes without publishing any values is an
error. `Return_Value_Data_Source` retains its existing empty-queue `Return`
behavior, but the preprocessor drains its queue by size so a literal returned
string `Return` remains data.

## Publishing from C++

A module keeps `void Interpreter(Data_Source *Data)`. Its returning command
handler publishes the C++ result explicitly:

```cpp
else if(Command == "Mouse_X")
{
    int Value = Mouse_X();
    Data->Return_Value(std::to_string(Value));
}
```

Call `Return_Value` multiple times to expose multiple properties. Publish strings
directly and convert numeric values to strings, matching the existing token-based
argument conversions. A regular, non-expression command discards incidental C++
results. A C++ function's return value is not discovered automatically: its module
handler must publish it. Input's existing getters have been updated accordingly.

The Data_Source interface copies in the bundled module sources include the new
virtual methods. Rebuild modules against these headers, especially any external
module that implements its own Data_Source subclass. Updating a header does not
update an already-built DLL or SO.

## Appeal functions

Definitions are still supported and are captured without expanding variables or
executing expressions. Every call gets its own read cursor over the definition.

```text
Define_Function Position
    Return_Value(Input Mouse_X)
End_Function

Print(Call Position)
```

`Return_Value` publishes one argument and continues execution. Repeating it
publishes more properties. `Return` ends the script as before; reaching a file's
end also returns to its caller. A separate script containing `Return_Value 42`
can be used as `Print(Run path/to/script.Appeal)`.

Expressions in loop conditions are retained and reevaluated on each test, just
like loop variable references. Expressions in captured blocks run only when the
block executes. A condition expression must return exactly one value.

## Source contract and errors

Sources provide complete raw tokens through `Get_Data()`, preserving quotes.
For example, `"text (not code)"` must retain its quotes until preprocessing.
The file adapter now does this itself; generic File_IO behavior is unchanged.
Parentheses can be attached to a token. Physical lines and TCP packet boundaries
have no meaning to the preprocessor, and semicolons are not introduced here.

A TCP adapter must assemble complete tokens (including quoted strings) across
packets and wait for incoming input in its own read implementation. It must not
report `Return` merely because no packet is available yet. On actual end of its
script it can emit `Return`; `Done` retains its program-stop meaning. Sources can
override `End_Of_Source()` to distinguish physical exhaustion from a literal
`Return` command when capturing a function or block.

Malformed parentheses, missing arguments, missing variables, unused returned
properties, and expressions without results stop execution with an error instead
of reading an empty queue or continuing into the caller's arguments. Temporary
expression sources are removed during error unwinding. `Done` is rejected inside
an expression; use normal source completion or `Return` in a called script.

## Verification

Run `make -f Makefile test` from the repository root. The integration tests cover
nested and multiple results, module handlers, repeated Appeal calls, deferred
blocks, loop expressions, callbacks, quoted strings, file switching, and errors.

On Windows, the test executable can also check the rebuilt Input DLL with a
deterministic getter (no hardware input required):

```powershell
.\Obj\Windows\Preprocessor_Tests.exe .\Resources\Modules\Appeal_Module_Input\Appeal_Module_Input.dll
```

The forward-only source test verifies the token interface, not a live TCP session.
