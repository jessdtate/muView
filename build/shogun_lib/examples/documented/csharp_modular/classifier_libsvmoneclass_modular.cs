// In this example a one-class support vector machine classifier is trained on a
// toy data set. The training algorithm finds a hyperplane in the RKHS which
// separates the training data from the origin. The one-class classifier is
// typically used to estimate the support of a high-dimesnional distribution. 
// For more details see e.g. 
//   B. Schoelkopf et al. Estimating the support of a high-dimensional
//   distribution. Neural Computation, 13, 2001, 1443-1471. 
// 
// In the example, the one-class SVM is trained by the LIBSVM solver with the
// regularization parameter C=1 and the Gaussian kernel of width 2.1 and the
// precision parameter epsilon=1e-5.
// 
// For more details on LIBSVM solver see http://www.csie.ntu.edu.tw/~cjlin/libsvm/

using System;

public class classifier_libsvmoneclass_modular {
	public static void Main() {
		modshogun.init_shogun_with_defaults();
		double width = 2.1;
		double epsilon = 1e-5;
		double C = 1.0;

		double[,] traindata_real = Load.load_numbers("../data/fm_train_real.dat");
		double[,] testdata_real = Load.load_numbers("../data/fm_test_real.dat");

		RealFeatures feats_train = new RealFeatures();
		feats_train.set_feature_matrix(traindata_real);
		RealFeatures feats_test = new RealFeatures();
		feats_test.set_feature_matrix(testdata_real);

		GaussianKernel kernel = new GaussianKernel(feats_train, feats_train, width);

		LibSVMOneClass svm = new LibSVMOneClass(C, kernel);
		svm.set_epsilon(epsilon);
		svm.train();

		kernel.init(feats_train, feats_test);
		double[] out_labels = BinaryLabels.obtain_from_generic(svm.apply()).get_labels();

		foreach (double item in out_labels)
		    Console.Write(item);

		modshogun.exit_shogun();
	}
}
