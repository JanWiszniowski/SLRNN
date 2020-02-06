# SLRNN
The Single Layer Recurrent Neural Network (SLRNN) consists of a single layer of neurons. Outputs of all neurons are recurrently connected to their inputs. Each recurrent signal is delayed by a certain number of network operating steps. It was applied to detect seismic events. It is described by Doubravova et al. (2016). In the current version, new inputs are added to the network, the selection of input parameters including polarization factors is flexible. the training method was modified by adding retraining. Training data preparation and detection are written in C++ whereas the training itself, some test, and data visualisation are in Matlab. The program in C++ is a plugin to the SWIP5 program (will be available soon), which support seismic waveform and seismic catalog reading as well as GUI for the SLRNN detection plugin. The plugin prepares a training set base on three types of data: seismic waves with picked phases, invisible waves without picked phases, and noise. Seismic bulletins, continuous recording data, and a set of noticeable examples of noise are sources of training data. The QuakeML catalog is used, continuous seismic data served by the ArcLink and a text file containing list od station and time where noises or false detections were found.

The compilation of the plugin requires (Visual Studio 2013) and SWIP5 environment (will be available soon). It uses open source libraries: lboost 1.58.0, wxWidgets 3.0.2, armadillo 8.400.0

Thy Matlab codes run on Matlab 2015a.

The repository includes;

Catalog SVN: The source code of the SWIP5 plugin for data preparation and seismic events detection.

Catalog MATLAB: The source code for training the SLRNN, visualization of results, preparing data for events detection by the plugin.

dat3: The configuration of the plugin for data preparation, weights for detection seismic events by the SLRNN, and seismic events detection and a training data set.

References:

Doubravová, J., J. Wiszniowski, and J. Horálek (2016). Single Layer Recurrent Neural Network for detection of swarm-like earthquakes in W-Bohemia/Vogtland—the method. Computers & Geosciences 93 138-149, DOI:10.1016/j.cageo.2016.05.01105.011.
