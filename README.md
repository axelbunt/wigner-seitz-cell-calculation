# Wigner-Seitz cell calculation

## Formal task
The number of atoms of the cell and their coordinates in space are entered. Output the coordinates of the Wigner-Seitz cell and its area.

## Tests 

| Input                                       | Output                                                                                                                |
|---------------------------------------------|-----------------------------------------------------------------------------------------------------------------------|
| 4<br>0 0<br>1 4<br>6 4<br>5 0<br><br><br>   | x: 1.5, y: -2.5<br>x: 2.5, y: -1.5<br>x: 2.5, y: 1.5<br>x: -1.5, y: 2.5<br>x: -2.5, y: 1.5<br>x: -2.5, y: -1.5 <br>20 |   
| 5<br>0 0<br>2 2<br>2 -2<br> -2 -2<br> -2 2  | x: 0, y: -2<br>x: 2, y: 0<br>x: 0, y: 2<br>x: -2, y: 0<br>8<br><br>                                                   |
| 4<br>0 0<br>0 2<br>2 2<br>2 0               | x: 1, y: -1<br>x: 1, y: 1<br>x: -1, y: 1<br>x: -1, y: -1<br>4                                                         |
## Visualisations of tests

- *Blue dots*: entered atoms
- *Red dots*: calculated neighbors of 'initial' atom
- *Blue area*: calculated Wigner-Seitz Cell

Test 1 visualisation:  
![test_1.png](https://raw.githubusercontent.com/axelbunt/wigner-seitz-cell-calculation/main/assets/test_1.png)

Test 2 visualisation:  
![test_2.png](https://raw.githubusercontent.com/axelbunt/wigner-seitz-cell-calculation/main/assets/test_2.png)

Test 3 visualisation:  
![test_3.png](https://raw.githubusercontent.com/axelbunt/wigner-seitz-cell-calculation/main/assets/test_3.png)  

## Links
[Project presentation](https://docs.google.com/presentation/d/1fOl5tqzPE-S7Qgi6AVF63OB6G5AeXLH3dSqGM8U1Rls/edit?usp=sharing)  
[Wigner Seitz Cell](https://en.wikipedia.org/wiki/Wigner%E2%80%93Seitz_cell)
