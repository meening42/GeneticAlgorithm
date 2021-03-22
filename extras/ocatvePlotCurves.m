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

x =     [10,20,30,40, 45, 50, 55, 60, 65   70,  75,  80,  85   90,  100]
y2010 = [85,33,119,237,241,396,678,1081,1150,1373,2017,2870,3466,3098,1765]
y2015 = [52,40,99,211,183,314,577,884,1357,1501,1895,2553,3614,3655,2899]
y2019 = [61,28,84,196,159,252,462,772,1274,1819,1867,2594,3293,3963,3791]
y=[y2010; y2015 ; y2019]'
length(x)
length(y)
figure(2)
bar(x,y)