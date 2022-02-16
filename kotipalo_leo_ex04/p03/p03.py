import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe

O0 = np.loadtxt('O0.tsv')
O3 = np.loadtxt('O3.tsv')

plt.figure()
(_, minmax, mean, var, _, _) = describe(O0)
plt.bar(['1, 0', '2, 0', '4, 0', '8, 0', '16, 0', '32, 0'], mean, yerr=np.sqrt(var))
(_, minmax, mean, var, _, _) = describe(O3)
plt.bar(['1, 3', '2, 3', '4, 3', '8, 3', '16, 3', '32, 3'], mean, yerr=np.sqrt(var))
plt.savefig("p03.png")