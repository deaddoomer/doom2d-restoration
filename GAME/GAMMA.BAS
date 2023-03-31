OPEN "GAMMA.DAT" FOR OUTPUT AS 1
FOR G = 1! TO 2! STEP .25
  PRINT #1, "{";
  FOR I = 0 TO 63
    W% = INT(((I / 63!) ^ (1! / G)) * 63)
    PRINT #1, USING "##"; W%;
    IF I < 63 THEN PRINT #1, ",";
  NEXT
  PRINT #1, "}";
  IF G <> 2 THEN PRINT #1, ","
NEXT
PRINT #1, ""
CLOSE 1
END

