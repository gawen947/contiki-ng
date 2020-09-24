import sys

# argv1 = file
# argv2 = bin_size

histogram = {}
total = 0

print "# bin = exec/sleep period in microsec"
print "# bin_min bin_max occurences frq"

bin_size = int(sys.argv[2])
with open(sys.argv[1], 'r') as f:
    for line in f:
        v = int(line)

        bin_idx = v // bin_size
        if bin_idx not in histogram:
            histogram[bin_idx] = 0
        histogram[bin_idx] += 1
        total += 1

for bin_idx in sorted(histogram):
    bin_val_min = bin_idx * bin_size
    bin_val_max = (bin_idx + 1) * bin_size
    occur = histogram[bin_idx]
    frq = 100. * float(occur) / total
    print bin_val_min, bin_val_max, occur, frq
