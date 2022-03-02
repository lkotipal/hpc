import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe

a = np.stack(
	(
		np.loadtxt('no-omit-0.tsv', unpack=True)[1], 
		np.loadtxt('omit-0.tsv', unpack=True)[1], 
		np.loadtxt('no-omit-1.tsv', unpack=True)[1],
		np.loadtxt('omit-1.tsv', unpack=True)[1], 
		np.loadtxt('no-omit-2.tsv', unpack=True)[1],
		np.loadtxt('omit-2.tsv', unpack=True)[1], 
	)
).transpose()

plt.figure()
(_, _, mean, var, _, _) = describe(a)
plt.bar(['no-omit, O0', 'omit, O0', 'no-omit, O1', 'omit, O1', 'no-omit, O2', 'omit, O2',], mean, yerr=np.sqrt(var))
plt.ylabel(r'$t (s)$')
plt.savefig("p02.png")