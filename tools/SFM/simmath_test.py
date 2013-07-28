import simmath

pairs = [[0.0, 4.0], [1.0, 4.8], [2.0, 6.0]]
print(pairs)
coeffs = simmath.approximateByPolynom(pairs)
print(coeffs)

print(simmath.evalPolynom(coeffs, 0.0))
print(simmath.evalPolynom(coeffs, 1.0))
print(simmath.evalPolynom(coeffs, 2.0))