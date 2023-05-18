import matplotlib.pyplot as plt
# raw data
blocksz = [1, 2, 3, 4, 5, 6 ]
reraor = [18.74, 50.00, 299.00, 479, 685.00, 872.85]
rerara = [6.25, 25, 85, 266, 600, 834]
wrraor = [18.74, 50, 300, 480, 564, 650]
wrrara = [5.53, 18.75, 66.67, 218.1, 417.4, 609.5]
redior = [12.5, 75, 300, 149, 564, 752]
redira = [2.67, 4.1, 5.82, 20.0, 290.8, 463]
wrdior = [18.75, 75, 200, 133, 220, 253]
wrdira = [0.48, 1.47, 4.25, 25, 70, 194]

#plt.xticks([1, 2, 3, 4, 5, 6], ['64', '256', '1024', '4096', '16384', '65536'])

fig, axes = plt.subplots(nrows=2, ncols=2, figsize=(10, 6))
axes[0, 0].set_xticks([1, 2, 3, 4, 5, 6], ['64', '256', '1024', '4096', '16384', '65536'])
axes[0, 0].plot(blocksz, reraor, label = "RAM ORDERED READ")
axes[0, 0].plot(blocksz, redior, label = "SSD ORDERED READ")
axes[0, 0].set_xlabel("BLOCK SIZE (Byte)")
axes[0, 0].set_ylabel("THROUGHPUT (MB/s)")
axes[0, 0].set_title("ORDERED READ THROUGHPUT")
axes[0, 0].legend()

axes[0, 1].set_xticks([1, 2, 3, 4, 5, 6], ['64', '256', '1024', '4096', '16384', '65536'])
axes[0, 1].plot(blocksz, rerara, label = "RAM RANDOM READ")
axes[0, 1].plot(blocksz, redira, label = "SSD RANDOM READ")
axes[0, 1].set_xlabel("BLOCK SIZE (Byte)")
axes[0, 1].set_ylabel("THROUGHPUT (MB/s)")
axes[0, 1].set_title("RANDOM READ THROUGHPUT")
axes[0, 1].legend()

axes[1, 0].set_xticks([1, 2, 3, 4, 5, 6], ['64', '256', '1024', '4096', '16384', '65536'])
axes[1, 0].plot(blocksz, wrraor, label = "RAM ORDERED WRITE")
axes[1, 0].plot(blocksz, wrdior, label = "SSD ORDERED WRITE")
axes[1, 0].set_xlabel("BLOCK SIZE (Byte)")
axes[1, 0].set_ylabel("THROUGHPUT (MB/s)")
axes[1, 0].set_title("ORDERED WRITE THROUGHPUT")
axes[1, 0].legend()

axes[1, 1].set_xticks([1, 2, 3, 4, 5, 6], ['64', '256', '1024', '4096', '16384', '65536'])
axes[1, 1].plot(blocksz, wrrara, label = "RAM RANDOM WRITE")
axes[1, 1].plot(blocksz, wrdira, label = "SSD RANDOM WRITE")
axes[1, 1].set_xlabel("BLOCK SIZE (Byte)")
axes[1, 1].set_ylabel("THROUGHPUT (MB/s)")
axes[1, 1].set_title("RANDOM WRITE THROUGHPUT")
axes[1, 1].legend()
#plt.show()
#plt.legend()
plt.tight_layout()
plt.savefig("fig.png")
