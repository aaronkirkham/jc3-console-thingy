## Just Cause 3 Console Thingy
A Just Cause 3 port of my [console thingy for Just Cause 4](https://github.com/aaronkirkham/jc4-console-thingy). Spawn anything and everything, vehicles, characters, animals, props.

<p align="center"><img src="https://i.imgur.com/gUAmqcR.png" alt=""></p>

### How to Install
 - Download the latest version from [releases](https://github.com/aaronkirkham/jc3-console-thingy/releases)
 - Extract `xinput9_1_0.dll` to your Just Cause 3 installation folder
 - Start the game and press the tilde (\`~) or F1 key to open the console
 - ***OPTIONAL*** - Enable faster game loading with the `-quickstart` launch argument via Steam

### Commands
 - `spawn` - Spawn everything! Vehicles, characters, animals & props.
 - `event` - Trigger game events - **CAUTION** - Stick to using only the hints, unless you know what the event does
 - `world` - Change world parameters

### Controls
 - `Tilde` (\`~) or `F1` - Toggle the input box
 - `Escape` - Clear current input text
 - `Up/Down Arrows` - Navigate the input history
 - `TAB` - Change focus to the hints list
 - **When hints list is focused**
   - `Escape` - Return focus to main input
   - `Up/Down Arrows` - Navigate the list
   - `Enter` - Append the current hint to the input text

#### Building
If you want to compile the code yourself, you will need **Visual Studio 2017 or later** (or a compiler which supports C++17 nested namespaces)
 - Clone this repository
 - Run `configure.ps1` with PowerShell
 - Build `out/jc3-console-thing.sln` in Visual Studio
