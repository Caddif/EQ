# EQ
## Table of contents
* [Overview](#overview)
* [Technologies](#technologies)
* [Screenshots](#screenshots)

## Overview
The goal of this project was to create an equalizer with added noise gate and compressor. It is a VST3 deploy.
The filter types in the equalizer are: <br />
•	HPF and LPF: Cutting the range above or below a certqain frequency.<br />
•	Peak filter: As the name suggests the filter boosts the sound at a certain frequency (or attenuates if the gain is negative).<br />
•	Low shelf: The range of operation of this filter acts similar to an LPF, meaning that any frequency below the one chosen by the user is affected. However, instead of cutting them, we have control over the gain of frequencies. We can once again boost or attenuate the chosen range. <br />
•	High shelf: Acts similarly to a low shelf but the range affected is above the chosen frequency.<br />
•	Notch filter: Sometimes referred to as the band reject filter which is a lot more descriptive name. The notch filter is responsible for cutting the exact frequency out of the signal. Three of the above filters used all parameters mentioned earlier. In the case of notch filtering gain value is not considered, the user has control over the frequency and quality of the filter.


## Technologies
* C++
* JUCE

## Screenshots
![image](https://user-images.githubusercontent.com/72463980/221174962-8ad7a592-dec2-4b70-934c-b337f55ac3d1.png)
