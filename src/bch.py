import numpy as np

# (Assume G is the exact same G matrix from the previous script)
G = np.array([
    [ 0.5000,  0.5000,  0.5000,  0.5000],
    [ 0.8660, -0.2500, -0.2500, -0.8660],
    [ 0.5000, -0.5000, -0.5000,  0.5000],
    [-0.0000,  1.0000, -1.0000,  0.0000],
    [ 0.5000, -0.5000, -0.5000,  0.5000],
    [ 0.8660, -0.2500, -0.2500, -0.8660]
])

G_pinv = np.linalg.pinv(G)

print("const float DECODE_MATRIX[4][6] = {")
for row in G_pinv:
    print("    {" + ", ".join([f"{x: .4f}" for x in row]) + "},")
print("};")
