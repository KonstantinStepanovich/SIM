import math
import simmath

def evalBPols(polars):
	result = []
	for polar in polars:
		pairs = [[polar[2][1], polar[2][0] - polar[1]], [polar[3][1], polar[3][0] - polar[1]]]
		result = result + [ polar[0], 
							simmath.approximateByPolynomPwr(pairs, [2, 4]) ]
	return result
	
def evalKDr(absPairs, Ce):
	relPairs = []
	for absPair in absPairs:
		relPairs = relPairs + [ [ absPair[0], absPair[1] / Ce ] ]
	return simmath.approximateByPolynom(relPairs)

def printLines(list):
	for line in list:
		print(line)