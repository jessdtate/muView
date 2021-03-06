% In this example a support vector regression algorithm is trained on a
% real-valued toy data set. The underlying library used for the SVR training is
% SVM^light. The SVR is trained with regularization parameter C=1 and a gaussian
% kernel with width=2.1. The the label of both the train and the test data are
% fetched via svr.classify().get_labels().
% 
% For more details on the SVM^light see
%  T. Joachims. Making large-scale SVM learning practical. In Advances in Kernel
%  Methods -- Support Vector Learning, pages 169-184. MIT Press, Cambridge, MA USA, 1999.

init_shogun

addpath('tools');
label_train=load_matrix('../data/label_train_twoclass.dat');
fm_train=load_matrix('../data/fm_train_real.dat');
fm_test=load_matrix('../data/fm_test_real.dat');

% svmlight based support vector regression
try
	disp('SVRLight')

	feats_train=RealFeatures(fm_train);
	feats_test=RealFeatures(fm_test);
	width=2.1;
	kernel=GaussianKernel(feats_train, feats_train, width);

	C=1.2;
	epsilon=1e-5;
	tube_epsilon=1e-2;
	num_threads=3;
	labels=RegressioLabels(label_train);

	svr=SVRLight(C, epsilon, kernel, labels);
	svr.set_tube_epsilon(tube_epsilon);
	svr.parallel.set_num_threads(num_threads);
	svr.train();

	kernel.init(feats_train, feats_test);
	svr.apply().get_labels();
catch
	disp('No support for SVRLight available.')
end
