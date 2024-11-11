# RA-Assignment2

This project corresponds to the second assignment of the Randomized Algorithms course from the MIRI master's program at UPC.

## Usage Instructions

1. **Compile the Program**  
   First, compile the program using the command:

```bash
make all
```

2. **Run the Program**  
Execute the program with the following command:

```bash
./galtonBoard [-D|-B] P1 [-S|-K] P2
```

- **[-D]** and **[-B]** are mutually exclusive options:
  - **-D** expects a value \( d > 1 \) for the d-choice scheme.
  - **-B** expects a real number \( \beta \) in the range (0,1) for the (1+β)-choice scheme.

- **[-S]** and **[-K]** are optional, but mutually exclusive:
  - **-S** expects the size of the batch for batch processing.
  - **-K** expects the number of questions [1,2] for the partial information scheme.
  - If neither option is provided, the program defaults to batch size = 1 with no questions (i.e., no uncertainty).

3. **Output**  
The simulation results will be saved in the folder `Simulations` with the filename format: 
```bash
[D|B]P1_[S|K|0]P2.csv
```