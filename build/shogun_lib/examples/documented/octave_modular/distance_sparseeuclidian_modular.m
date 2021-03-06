% This example shows how to compute the Eucledian Distance using a sparse encoding.

init_shogun

addpath('tools');
fm_train_real=load_matrix('../data/fm_train_real.dat');
fm_test_real=load_matrix('../data/fm_test_real.dat');

% sparse euclidian distance
disp('SparseEuclidianDistance')

realfeat=RealFeatures(fm_train_real);
feats_train=SparseRealFeatures();
feats_train.obtain_from_simple(realfeat);
realfeat=RealFeatures(fm_test_real);
feats_test=SparseRealFeatures();
feats_test.obtain_from_simple(realfeat);

distance=SparseEuclidianDistance(feats_train, feats_train);

dm_train=distance.get_distance_matrix();
distance.init(feats_train, feats_test);
dm_test=distance.get_distance_matrix();
