import numpy as np

for i in range(4):
	print(f"O{i}: mean {np.mean(np.loadtxt(f'O{i}.txt'))}s")