import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe
from scipy.optimize import curve_fit

x = np.linspace(0, 1E8, 51) * 4E-6	# MB
y = np.zeros(51)
yerr = np.zeros(51)
for i in range(51):
	a = np.loadtxt(f'times_{i}.tsv')
	(_, _, y[i], var, _, _) = describe(a)
	yerr[i] = np.sqrt(var)

(a, b), _ = curve_fit(lambda x, a, b: a * x + b, xdata=x, ydata=y, sigma=yerr)
print(f'b = {(1/a)} MB/s')
print(f'l = {b} s')
plt.errorbar(x, y, yerr, fmt=' ', capsize=2.0)
plt.plot(x, a * x + b)
plt.xlabel(f'$N$ (MB)')
plt.ylabel(f'$t$ (s)')
plt.savefig('p04.png')