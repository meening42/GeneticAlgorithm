function  r = write2File(table, fileN)
  disp("Writing table");
  fid = fopen(fileN, "w");
  for i = 1 : size(table,1)
    for j = 1 : size(table,2)
      fprintf(fid, '%f ', table(i,j));
    endfor
    fprintf(fid, '\r\n');
  endfor
  fclose(fid);
  r = 1;
endfunction

