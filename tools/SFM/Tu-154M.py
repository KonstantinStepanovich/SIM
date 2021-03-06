import SFM

print('Tu-154M')
print('\nBPol2, BPol4 by M')
SFM.printLines(SFM.evalBPols(
	[
		[0.40, 0.024, [0.027, 0.4], [0.046, 0.8]],
		[0.60, 0.024, [0.030, 0.4], [0.054, 0.8]],
		[0.75, 0.025, [0.032, 0.4], [0.067, 0.8]],
		[0.80, 0.025, [0.034, 0.4], [0.049, 0.6]],
		[0.85, 0.026, [0.037, 0.4], [0.059, 0.6]],
		[0.90, 0.028, [0.045, 0.4], [0.070, 0.5]],
		[0.95, 0.040, [0.045, 0.2], [0.070, 0.4]]
	]
))

print('\nKDr')
print(
SFM.evalKDr(
	[[0.6, 0.078], [0.7, 0.056], [0.8, 0.05], [0.9, 0.047]],
	0.052
)
)