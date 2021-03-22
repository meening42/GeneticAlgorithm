x = -9:9;

a = 0.5;
b = 3.2;
c = 7.5;

y = a*x.^2 +b*x + c

a2 = 0.49
b2 = 3.3 
c2 = 7.4
y2 =  a2*x.^2 +b2*x + c2;
plot(x,y,'o',x,y2,'r')
grid;
legend("input data","Gen. Algo")

# output table 
inputTable = [x' y']
dlmwrite("inputTable.txt", inputTable)
#write2File(inputTable, "inputTable.txt")

