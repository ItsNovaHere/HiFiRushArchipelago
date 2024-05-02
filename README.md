# Hi-Fi RUSH support for Archipelago

This is a client mod for Hi-Fi RUSH that enables [Archipelago](https://archipelago.gg) integration.

It is currently in very early development.

# Build Instructions
1. Clone the repo wherever you wish.
2. Run `git submodule update --init --recursive` to clone all the submodules. You will need a github account with access to the UE4 source code to clone UE4SS.
3. Make the following edits to the modules:

	Open `deps\RE-UE4SS\UE4SS\xmake.lua` and comment out or remove lines 96 and 97, then add `local commit_hash = "A"` before the `target:add` call.

	Open `deps\apclientpp\apclient.hpp` and add `#undef min` at the end of all the defines.

	Open `deps\valijson\include\compat\optional.hpp` and add the following lines to the start of the file:

   		#pragma warning(disable:4583)
		#pragma warning(disable:4582)

4. Edit `mod\xmake.lua` line 42 to point to your game install.
5. You should now be able to run `xmake` and build the dll.
