#include <iostream>

#include "Interface/Interface.h"

#include "BoardRepresentation/Bitboard.h"
#include "Engine/MagicBitboardHelper.h"


#define TEST 1

int main() {

	//  MagicBitboardHelper mbh;

	//  return 0;

	Interface interface;

#if TEST
	interface.ProcessCommand("position startpos moves e2e4 b8c6 b1c3 g8f6 d2d4 e7e5 d4e5 c6e5 c1f4 e5c6 e4e5 c6e5 f4e5 d8e7 d1e2 e8d8 c3d5 f6d5 e1c1 d5b4 c1b1 e7e6 b2b3 e6f5 g2g4 f5g6 e2c4 c7c5 f1g2 b4c2");
	//interface.ProcessCommand("position startpos");
	interface.ProcessCommand("show");
	//interface.ProcessCommand("blackattack");
	//interface.ProcessCommand("pseudomoves");
	interface.ProcessCommand("moves");

	//interface.ProcessCommand("go depth 7");
	//interface.ProcessCommand("perft 5");

#else
	interface.ListenForConnection();
	interface.ListenForCommands();
#endif

	return 0;
}

/*
a2a3: 2661507
f2f3: 2016751
h2h3: 2545619
b3b4: 2823021
g4g5: 2581324
a2a4: 2575116
f2f4: 2294868
h2h4: 2823680
g1e2: 3163441
g1f3: 2652958
g1h3: 2737621
g2f1: 2279298
g2f3: 2294696
g2h3: 1957164
g2e4: 3058192
g2d5: 2163097
g2c6: 2551267
g2b7: 2589171
e5a1: 2411729
e5b2: 2509533
e5c3: 2588283
e5g3: 2250531
e5d4: 2230279
e5f4: 2417146
e5d6: 1631341
e5f6: 322571
e5c7: 192264
e5g7: 2875562
e5b8: 2256199
d1c1: 2202853
d1e1: 2427011
d1f1: 2051091
d1d2: 2563003
d1d3: 2959680
d1d4: 2152765
d1d5: 1963635
d1d6: 2106115
d1d7: 212239
c4f1: 1831789
c4c2: 1920605
c4e2: 2296038
c4c3: 2444115
c4d3: 2942686
c4a4: 2269404
c4b4: 2530801
c4d4: 2201812
c4e4: 2666868
c4f4: 2380462
c4b5: 2290627
c4c5: 2573899
c4d5: 2050509
c4a6: 2367385
c4e6: 1740479
c4f7: 2018785
b1b2: 3461015
b1c1: 3019871

Nodes searched: 129099771
*/