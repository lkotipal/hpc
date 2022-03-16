import numpy as np
import matplotlib.pyplot as plt
from math import log2, ceil
from scipy.stats import chi2_contingency

a = np.loadtxt('p01.tsv')[0]
pos = np.arange(1, 301)
pos1 = np.where(a==1, pos, 0)
pos2 = np.where(a==2, pos, 0)
pos3 = np.where(a==3, pos, 0)

nbins=int(ceil(log2(np.size(a)))) + 1
print(nbins)
(n1, _, _) = plt.hist(pos1, bins=nbins, range=(1, 300), alpha=0.7, label='Process 1')
(n2, _, _) = plt.hist(pos2, bins=nbins, range=(1, 300), alpha=0.7, label='Process 2')
(n3, _, _) = plt.hist(pos3, bins=nbins, range=(1, 300), alpha=0.7, label='Process 3')
plt.legend()
plt.xlabel('Position')
plt.ylabel('Occurrences')
plt.savefig('p01.png')

print(chi2_contingency(np.stack((n1, n2, n3))))