import pandas as pd
import os

# os.system('for i in {1..1000000} ; do ./baccarat --ugly-output >> /tmp/simulation_million.output; done')
# os.system('cp /tmp/simulation_million.output ./')
# os.system('tar cfJ simulation_million.output.tar.xz simulation_million.output')

os.system('sha256sum simulation_million.output.tar.xz')
# e6d0223acd1046695357d9c9ef19c6f2f0aba009cee593a9cbc7ed4ad620fcbe  simulation_million.output.tar.xz

os.system('tar xf simulation_million.output.tar.xz')

simulation_lines = [line.strip() for idx, line
                    in enumerate(open('./simulation_million.output'))
                    if idx % 2 == 1
                    ]

simulation_outcome = [s.split(' - ') for s in simulation_lines]

df = pd.DataFrame(simulation_outcome)
df = df.rename(columns={0: 'Player Hand', 1: 'Banker Hand', 2: 'Outcome'})

df.groupby(['Outcome']).size()
# Outcome
# BANKER    459157
# PLAYER    444936
# TIE        95907

# Wizard of Odds
# https://wizardofodds.com/games/baccarat/appendix/1/
# Banker Bet — 8 Decks
# EVENT	PAYS	COMBINATIONS	PROBABILITY	RETURN
# Banker wins	0.95	2,292,252,566,437,888	0.458597	0.435668
# Player wins	-1	2,230,518,282,592,256	0.446247	-0.446247
# Tie	0	475,627,426,473,216	0.095156	0
# Total	 	4,998,398,275,503,360	1	-0.010579

player_cards_columns = ['Player Card One', 'Player Card Two', 'Player Card Three']
banker_cards_columns = ['Banker Card One', 'Banker Card Two', 'Banker Card Three']
df[player_cards_columns] = df['Player Hand'].str.split(',', expand=True)
df[banker_cards_columns] = df['Banker Hand'].str.split(',', expand=True)

df['Player Cards'] = [2 if third_card is None else 3 for third_card in df['Player Card Three']]
df['Banker Cards'] = [2 if third_card is None else 3 for third_card in df['Banker Card Three']]

card_values = {'A': 1, '2': 2, '3': 3, '4': 4, '5': 5, '6': 6, '7': 7, '8': 8, '9': 9, '10': 0, 'J': 0, 'Q': 0, 'K': 0, 'None': 0}

def valuate_card(card_value: str) -> int:
    try:
        return card_values[card_value]
    except KeyError:
        return 0

df['Player Hand Score'] = df[player_cards_columns].applymap(valuate_card).sum(axis=1) % 10
df['Banker Hand Score'] = df[banker_cards_columns].applymap(valuate_card).sum(axis=1) % 10

df.groupby(['Player Hand Score']).count()['Player Hand']
# Player Hand Score
# 0     94293
# 1     74204
# 2     74633
# 3     74300
# 4     74707
# 5     74585
# 6    132667
# 7    134165
# 8    132824
# 9    133622

# Wizard of Odds
# Total Player Points by Number of Cards - 8 Decks
# TOTAL
# POINTS	2 CARDS	3 CARDS	TOTAL
# 0	0.027939	0.066051	0.093989
# 1	0.017984	0.056544	0.074527
# 2	0.017913	0.056404	0.074317
# 3	0.017984	0.056544	0.074527
# 4	0.017914	0.056395	0.074309
# 5	0.017984	0.056527	0.074511
# 6	0.094532	0.038695	0.133227
# 7	0.094903	0.038764	0.133666
# 8	0.094532	0.038707	0.133239
# 9	0.094903	0.038784	0.133687
# Total	0.496586	0.503414	1


df.groupby(['Banker Hand Score']).count()['Banker Hand']
# Banker Hand Score
# 0     88266
# 1     69003
# 2     68730
# 3     73003
# 4     93606
# 5    100319
# 6    121194
# 7    128698
# 8    128223
# 9    128958

# Wizard of Odds
# Total Banker Points by Number of Cards - 8 Decks
# TOTAL
# POINTS	2 CARDS	3 CARDS	TOTAL
# 0	0.027939	0.060837	0.088775
# 1	0.017984	0.051298	0.069281
# 2	0.017913	0.05114	    0.069054
# 3	0.022527	0.050249	0.072776
# 4	0.049551	0.043879	0.093429
# 5	0.058816	0.041888	0.100704
# 6	0.085529	0.035547	0.121076
# 7	0.094903	0.033529	0.128431
# 8	0.094532	0.033501	0.128033
# 9	0.094903	0.033538	0.12844
# Total	0.564595	0.435405	1

df.to_csv('simulation_million.csv')

import dtale
dtale.show(df, subprocess=True)
