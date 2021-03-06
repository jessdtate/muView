// This is an example initializing the oligo string kernel which takes distances 
// between matching oligos (k-mers) into account via a gaussian. Variable 'k' defines the length 
// of the oligo and variable 'w' the width of the gaussian. The oligo string kernel is 
// implemented for the DNA-alphabet 'ACGT'. 
//  

using System;

public class kernel_oligo_string_modular {
	public static void Main() {
		modshogun.init_shogun_with_defaults();
		int size_cache = 3;
		int k = 1;
		double width = 10;

		String[] fm_train_dna = Load.load_dna("../data/fm_train_dna.dat");
		String[] fm_test_dna = Load.load_dna("../data/fm_test_dna.dat");

		StringCharFeatures feats_train = new StringCharFeatures(fm_train_dna, EAlphabet.DNA);
		StringCharFeatures feats_test = new StringCharFeatures(fm_test_dna, EAlphabet.DNA);

		OligoStringKernel kernel = new OligoStringKernel(size_cache, k, width);
		kernel.init(feats_train, feats_train);

		double[,] km_train = kernel.get_kernel_matrix();
		kernel.init(feats_train, feats_test);
		double[,] km_test = kernel.get_kernel_matrix();
		modshogun.exit_shogun();
	}
}