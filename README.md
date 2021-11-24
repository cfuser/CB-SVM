The project is about CB-SVM, homework of data mining.



The project is compiled by Visual Studio 2017

First, generated_data.exe to generated data in data.txt;

Then, split.exe to split data.txt to train.txt and test.txt;

CB_SVM.exe and standard_SVM.exe to generated model in CF_tree.model and standard_train.model

predict_evalutaion.exe to test the accuracy of model by test.txt

---

CF.h and CF.cpp is the struct of Clustered Tree.

MVN.h is to generate MultiNormal Data.

CB_SVM.h is the struct of CB_SVM tree.

---

data_generated.cpp store in data.txt
data.txt
n m
data point
n is the number of point, m is the dimension including label

- - -
data_generted.cpp
MVN.h
to
data_generated.exe

- - -
standard_svm.cpp
svm.cpp
my_svm.h
svm.h
to
standard_svm.exe

- - -
main.cpp
CF.cpp
CF.h
to
CF.exe

- - -
model_evaluation.cpp
to
model_evaluation.exe

---

split.cpp

to

split.exe

