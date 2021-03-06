import classifier_multiclass_shared
# run with toy data
[traindat, label_traindat, testdat, label_testdat] = classifier_multiclass_shared.prepare_data()

parameter_list = [[traindat,testdat,label_traindat,label_testdat,2.1,1,1e-5],[traindat,testdat,label_traindat,label_testdat,2.2,1,1e-5]]

def classifier_multiclasslinearmachine_modular (fm_train_real=traindat,fm_test_real=testdat,label_train_multiclass=label_traindat,label_test_multiclass=label_testdat,lawidth=2.1,C=1,epsilon=1e-5):
	from shogun.Features import RealFeatures, MulticlassLabels
	from shogun.Classifier import LibLinear, L2R_L2LOSS_SVC, LinearMulticlassMachine
	from shogun.Classifier import ECOCStrategy, ECOCOVREncoder, ECOCLLBDecoder, MulticlassOneVsRestStrategy

	feats_train = RealFeatures(fm_train_real)
	feats_test  = RealFeatures(fm_test_real)

	labels = MulticlassLabels(label_train_multiclass)

	classifier = LibLinear(L2R_L2LOSS_SVC)
	classifier.set_epsilon(epsilon)
	classifier.set_bias_enabled(True)

	mc_classifier = LinearMulticlassMachine(MulticlassOneVsRestStrategy(), feats_train, classifier, labels)
	mc_classifier.train()
	label_mc = mc_classifier.apply(feats_test)
	out_mc = label_mc.get_labels()

	ecoc_strategy = ECOCStrategy(ECOCOVREncoder(), ECOCLLBDecoder())
	ecoc_classifier = LinearMulticlassMachine(ecoc_strategy, feats_train, classifier, labels)
	ecoc_classifier.train()
	label_ecoc = ecoc_classifier.apply(feats_test)
	out_ecoc = label_ecoc.get_labels() 

	n_diff = (out_mc != out_ecoc).sum()
	if n_diff == 0:
		print("Same results for OvR and ECOCOvR")
	else:
		print("Different results for OvR and ECOCOvR (%d out of %d are different)" % (n_diff, len(out_mc)))

	if label_test_multiclass is not None:
		from shogun.Evaluation import MulticlassAccuracy
		labels_test = MulticlassLabels(label_test_multiclass)
		evaluator = MulticlassAccuracy()
		acc_mc = evaluator.evaluate(label_mc, labels_test)
		acc_ecoc = evaluator.evaluate(label_ecoc, labels_test)
		print('Normal OVR Accuracy = %.4f' % acc_mc)
		print('ECOC OVR Accuracy   = %.4f' % acc_ecoc)

	return out_ecoc, out_mc

if __name__=='__main__':
	print('MulticlassMachine')
	classifier_multiclasslinearmachine_modular(*parameter_list[0])

