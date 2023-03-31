OPEN "NOTETAB.DAT" FOR OUTPUT AS 1
MK# = 2 ^ (1 / 12)
N# = 1 / 4
C = 0
DO
W& = INT(N# * 1024)
PRINT #1, W&; ",";
C = C + 1
N# = N# * MK#
LOOP WHILE C < 12 * 8
CLOSE 1
END

