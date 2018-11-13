[mvx_o, mvy_o, type_o] = loadmvs('original.dump');
plot2d(lsbplane(mvx_o(:, :, 1)))
[mvx_h, mvy_h, type_h] = loadmvs('hidenseek.dump');
plot2d(lsbplane(mvx_h(:, :, 1)))
[mvx_r, mvy_r, type_r] = loadmvs('rand-hidenseek.dump');
plot2d(lsbplane(mvx_r(:, :, 1)))
[mvx_m, mvy_m, type_m] = loadmvs('mvsteg.dump');
plot2d(lsbplane(mvx_m(:, :, 1)))