import numpy as np

A = np.loadtxt("freqs.txt")
print(np.mean(A, axis=0))
print(np.min(A, axis=0))
print(np.max(A, axis=0))