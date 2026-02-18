#include <iostream>

#include "Interface/Interface.h"


int main() {
	Interface interface;

	interface.ListenForConnection();
	interface.ListenForCommands();

	return 0;
}
