# TAP_Analyzer
TechnoAP list data analyzer for USB-MCA.
The list data must be converted to CSV file by TechnoAP original software.

Current version create TTree file and output coincidence analyzed data in console.

Preparation:
Put this macro and your converted list data (csv format) in the same folder.
Edit TAP_Analyzer.c to suit your filename.
#define "xxxxxx.csv"

You can also set coincidence window in unit of ns by the following sentence:
#define COIN_WINDOW 1000

Usage:
Start ROOT (by CERN)

Command:
.x TAP_Analyzer.c

Output (number of Hits in a single event, pulse height of CH0, CH1, CH2, CH3, time of hit (ns) for CH0, CH1, CH2, CH3):

1 0 0 0 74 0 0 0 303570463880

2 0 68 0 126 0 303571606560 0 303571606320

1 0 0 0 63 0 0 0 303571750080

1 0 0 125 0 0 0 303573189120 0

1 0 393 0 0 0 303574627400 0 0

1 0 342 0 0 0 303575295240 0 0

3 0 119 159 191 0 303580460080 303580460040 303580459920

1 0 0 286 0 0 0 303581501040 0

1 0 630 0 0 0 303581648400 0 0


