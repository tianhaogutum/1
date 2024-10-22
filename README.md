# Gammakorrektur - Project Assignment for Computer Architecture Lab

## 1. Project Overview

This project addresses the implementation of an algorithm for gamma correction in image processing. The algorithm converts a color image to grayscale and subsequently performs gamma correction. The implementation is done in C following the C17 standard.

## 2. Prerequisites

- C Compiler (e.g., GCC)
- Git
- Netpbm library for image processing (optional)

## 3. Functionality

### 3.1 Grayscale Conversion

The conversion of a color pixel to grayscale is done by calculating a weighted average $begin:math:text$D$end:math:text$ using coefficients $begin:math:text$a$end:math:text$, $begin:math:text$b$end:math:text$, and $begin:math:text$c$end:math:text$:

$begin:math:display$
D = \\frac{a \\cdot R + b \\cdot G + c \\cdot B}{a + b + c}
$end:math:display$

The resulting grayscale image $begin:math:text$Q$end:math:text$ is defined by 

$begin:math:display$
Q(x,y) = D.
$end:math:display$

### 3.2 Gamma Correction

Gamma correction is performed using the following formula:

$begin:math:display$
Q'(x,y) = \\left(\\frac{Q(x,y)}{255}\\right)^{\\gamma} \\cdot 255.
$end:math:display$

## 4. Implementation

### 4.1 Main Function

The main function for performing gamma correction is defined as follows:

```c
void gamma_correct(
    const uint8_t* img, size_t width, size_t height,
    float a, float b, float c,
    float gamma,
    uint8_t* result);
```

### 4.2 I/O Operations

The program can read PPM files (24bpp PPM, P6) and save results in a user-defined Netpbm format.

## 5. Usage

### 5.1 Compilation

To compile the program, run the following command in the terminal:

```bash
gcc -o gamma_correction main.c -lm
```

### 5.2 Execution

The program can be executed with the following options:

```bash
./gamma_correction -V<Number> -B<Number> <InputFile> -o<OutputFile> --coeffs<a,b,c> --gamma<GammaValue>
```

- `-V<Number>`: Selects the implementation to use (e.g., `-V 0`).
- `-B<Number>`: (Optional) Measures the runtime of the implementation.
- `<InputFile>`: The name of the input file (PPM).
- `-o<OutputFile>`: The name of the output file (Netpbm format).
- `--coeffs<a,b,c>`: Coefficients for grayscale conversion (default values used if this option is not set).
- `--gamma<GammaValue>`: The gamma value for correction.
- `-h|--help`: Outputs a description of all options.

### 5.3 Example

A sample call might look like this:

```bash
./gamma_correction -V 0 -B 10 input.ppm -o output.ppm --coeffs 0.299,0.587,0.114 --gamma 2.2
```

## 6. Submission and Presentation

- **Submission Deadline**: Sunday, February 4, 2024, 23:59 CET
- **Presentation**: March 11 – March 15, 2024
- Slides for the presentation must be submitted in PDF format by the submission deadline.

## 7. Authors

- **Group**: Team140
- **Members**: [Insert member list here]

## 8. Contact

For questions or uncertainties, please reach out to your tutor or email [insert your email address here].

## 9. License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more information.
