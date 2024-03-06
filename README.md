# What is BASIC? 
The programming language **BASIC** — the name is an acronym for *Beginner’s All-purpose Symbolic Instruction Code* — was developed in the mid-1960s at Dartmouth College by John Kemeny and Thomas Kurtz.

In **BASIC**, a program consists of a sequence of numbered statements, as illustrated by the simple program below: 
```
10 REM Program to add two numbers
20 INPUT n1
30 INPUT n2
40 LET total = n1 + n2
50 PRINT total
60 END
```

# Features
## Overview
- **GUI Interaction**: Presents a user-friendly GUI with input and output interfaces.
- **Program Management**: Users can add, update, or delete statements through the input box or LOAD button, storing and displaying statements in the correct order.
- **Program Interpretation**: Correctly interprets BASIC programs, including expression parsing, evaluation, and statement execution. Displays runtime statistics in the syntax tree and maintains runtime context.
- **Error Handling**: Robustly handles errors in the input.
- **Object-Oriented Design**: Project is implemented with object-oriented design, featuring clear and readable code with appropriate comments.

## 1. Statements implemented in the minimal version of BASIC
![Alt text](image.png)
- **REM**
    - This statement is used for comments. Any text on the line after the keyword REM is ignored. 
- **LET**
    
    ```LET var = exp```
    - The LET keyword is followed by a variable name, an equal sign, and an expression. 
    - In BASIC, assignment is not an operator and may not be nested inside other expressions. 
- **PRINT**   
    
    ```PRINT exp```
    - where exp is an expression.
    - The effect of this statement is to print the value of the expression on the console and then print a newline character so that the output from the next PRINT statement begins on a new line.
- **INPUT**
   
    ```INPUT var```
    - where var is a variable read in from the user.
    - The effect of this statement is to print a prompt consisting of the string " ? " and then to read in a value to be stored in the variable. 
- **GOTO**

    ```GOTO n```
    - When the program hits this statement, the program continues from line n instead of continuing with the next statement. 
- **IF**

    ```IF exp1 op exp2 THEN n```
    - where exp1 and exp2 are expressions and op is one of the conditional operators
=, <, or >. If the condition holds, the program should continue from line n just as in the GOTO statement. If not, the program continues on to the next line.
- **END**

    ```END```
    - Marks the end of the program.


## 2. Expressions in BASIC
 In Minimal BASIC, the assignment operator is no longer part of the expression structure. The simplest expressions are variables and integer constants. These may be combined into larger expressions by enclosing an expression in parentheses or by joining two expressions with the operators +, -, *, /, **, and MOD. 

 **Explanation:**
 - In the expression LET r = a MOD b, the absolute value of r should be less than the absolute value of b, and the sign of r is the same as that of b. For example, 5 MOD 3 is 2 and 5 MOD (-3) is -1.
- ```exp1 ** exp2``` The exponentiation operator returns the result of exp1exp2, where exp1 and exp2 are expressions. The exponentiation operator is right associative, i.e., a ** b ** c is equal to a ** (b ** c). The operator has higher precedence than *, / and MOD.

## 3. Commands to control the BASIC interpreter
- **RUN**: Start program execution from the lowest-numbered line.
- **LOAD**: Load a file containing statements and commands.
- **LIST**: Display all entered codes in real time (previously implemented).
- **CLEAR**: Delete the current program.
- **HELP**: Provide a simple help message.
- **QUIT**: Exit from the BASIC interpreter.

## 4. Syntax Tree and Run Statistics Display
The syntax tree is an abstract representation of a program, where each statement can be represented as a tree. The structure of the syntax tree reflects the computation steps of the expression in the statement.

### Examples

1. **LET m = p + q*t**
   
    ```
    LET =
        m
        +
            p
            *
                q
                t
    ```

2. **IF m > max THEN n**

    ```
    IF THEN
        m
        >
        max
        n
    ```

3. **GOTO n**

    ```
    GOTO
    n
    ```

4. **PRINT p + q*t**

    ```
    PRINT
        +
            p
            *
                q
                t
    ```

### Runtime Statistics
After each execution, print the number of times each statement has been executed on the syntax tree. For example, for a GOTO statement executed five times: 
   `GOTO 5
   n`


## 5. Exception Handling
Utilize try/catch to handle syntax errors gracefully without crashing the interpreter.


# Technology Stack
- QT 4.11.1
- C++

## Installation
1. Clone the project to your local machine
   ```
   git clone https://github.com/your/repository.git
   ```
2. Open QT 4.11.1
3. Import the project files
4. Compile and run the project
