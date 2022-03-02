import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import describe
from scipy.optimize import curve_fit

x = np.array((50, 100, 200, 250, 400, 500, 1000, 2000))
y = np.zeros(8)
yerr = np.zeros(8)

for i in range(8):
	a = np.loadtxt(f'eigendouble_{x[i]}.tsv')
	(_, _, y[i], var, _, _) = describe(a)
	yerr[i] = np.sqrt(var)

(a, b), _ = curve_fit(lambda x, a, b: a * x**b, xdata=x, ydata=y, sigma=yerr)

print(f'a = {a}')
print(f'b = {b}')

plt.errorbar(x, y, yerr, fmt=' ', capsize=2.0, label='Data')
plt.plot(x, a * x**b, label=f'$a \cdot N^b$')
plt.legend()
plt.xscale('log', base=10)
plt.yscale('log', base=10)
plt.xlabel('N')
plt.ylabel('t')
plt.savefig('p01.png')