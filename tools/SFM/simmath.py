import math
from numpy import *
from numpy.linalg import *

def approximateByPolynom(pairs):
	a = []
	y = []
	for pair in pairs:
		row = []
		for pwr in range(len(pairs)):
			row = row + [ math.pow(pair[0], pwr) ]
		a = a + [ row ]
		y = y + [ [ pair[1] ] ]
	mx = solve(matrix(a), matrix(y))
	x = []
	for r in range(len(y)):
		x = x + [ mx[r, 0] ]
	return x
	
def approximateByPolynomPwr(pairs, pwrs):
	assert(len(pairs) == len(pwrs))
	a = []
	y = []
	for pair in pairs:
		row = []
		for pwr in pwrs:
			row = row + [ math.pow(pair[0], pwr) ]
		a = a + [ row ]
		y = y + [ [ pair[1] ] ]
	mx = solve(matrix(a), matrix(y))
	x = []
	for r in range(len(y)):
		x = x + [ mx[r, 0] ]
	return x
	
def evalPolynom(coeffs, x):
	y = 0
	for index in range(len(coeffs)):
		y = y + coeffs[index] * math.pow(x, index)
	return y