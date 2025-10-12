#
}
			// (no zone reassignment for passage cells)
#
i
n
c
l
u
d
e
 
"
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
.
h
"
#
i
n
c
l
u
d
e
 
"
D
r
a
w
D
e
b
u
g
H
e
l
p
e
r
s
.
h
"
#
i
n
c
l
u
d
e
 
"
A
l
g
o
/
R
a
n
d
o
m
S
h
u
f
f
l
e
.
h
"
#
i
n
c
l
u
d
e
 
"
D
i
g
E
m
p
i
r
e
/
M
a
p
/
M
a
p
G
r
i
d
2
D
.
h
"
b
o
o
l
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
G
e
n
e
r
a
t
e
(
U
M
a
p
G
r
i
d
2
D
*
 
M
a
p
G
r
i
d
,
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
c
o
n
s
t
 
T
A
r
r
a
y
<
i
n
t
3
2
>
&
 
Z
o
n
e
L
a
b
e
l
s
,
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
c
o
n
s
t
 
U
Z
o
n
e
B
o
r
d
e
r
S
e
t
t
i
n
g
s
*
 
S
e
t
t
i
n
g
s
)
{
	
i
f
 
(
!
V
a
l
i
d
a
t
e
I
n
p
u
t
s
(
M
a
p
G
r
i
d
,
 
Z
o
n
e
L
a
b
e
l
s
,
 
S
e
t
t
i
n
g
s
)
)
	
	
r
e
t
u
r
n
 
f
a
l
s
e
;
	
C
a
c
h
e
d
L
a
b
e
l
s
 
=
 
Z
o
n
e
L
a
b
e
l
s
;
	
C
a
c
h
e
d
S
i
z
e
 
=
 
M
a
p
G
r
i
d
-
>
G
e
t
S
i
z
e
(
)
;
	
P
a
s
s
a
g
e
s
.
R
e
s
e
t
(
)
;
	
/
/
 
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
.
c
p
p
 
(
i
n
s
i
d
e
 
G
e
n
e
r
a
t
e
)
	
T
M
a
p
<
F
I
n
t
P
o
i
n
t
,
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
>
 
P
a
i
r
T
o
A
,
 
P
a
i
r
T
o
B
;
	
C
o
l
l
e
c
t
Z
o
n
e
B
o
u
n
d
a
r
i
e
s
(
C
a
c
h
e
d
L
a
b
e
l
s
,
 
P
a
i
r
T
o
A
,
 
P
a
i
r
T
o
B
)
;
	
/
/
 
1
)
 
P
l
a
c
e
 
w
a
l
l
s
 
f
i
r
s
t
	
P
l
a
c
e
W
a
l
l
s
W
i
t
h
T
h
i
c
k
n
e
s
s
(
M
a
p
G
r
i
d
,
 
S
e
t
t
i
n
g
s
,
 
P
a
i
r
T
o
A
)
;
	
/
/
 
2
)
 
T
h
e
n
 
c
a
r
v
e
 
p
a
s
s
a
g
e
s
	
C
h
o
o
s
e
A
n
d
C
a
r
v
e
P
a
s
s
a
g
e
s
(
M
a
p
G
r
i
d
,
 
S
e
t
t
i
n
g
s
,
 
P
a
i
r
T
o
A
,
 
P
a
i
r
T
o
B
)
;
	
r
e
t
u
r
n
 
t
r
u
e
;
}
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
G
e
t
F
r
e
e
C
e
l
l
s
F
o
r
Z
o
n
e
(
U
M
a
p
G
r
i
d
2
D
*
 
M
a
p
G
r
i
d
,
 
i
n
t
3
2
 
Z
o
n
e
I
d
)
 
c
o
n
s
t
{
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
O
u
t
;
	
i
f
 
(
!
M
a
p
G
r
i
d
 
|
|
 
C
a
c
h
e
d
L
a
b
e
l
s
.
N
u
m
(
)
 
!
=
 
C
a
c
h
e
d
S
i
z
e
.
X
 
*
 
C
a
c
h
e
d
S
i
z
e
.
Y
)
 
r
e
t
u
r
n
 
O
u
t
;
	
i
f
 
(
Z
o
n
e
I
d
 
<
 
0
)
 
r
e
t
u
r
n
 
O
u
t
;
	
c
o
n
s
t
 
i
n
t
3
2
 
W
 
=
 
C
a
c
h
e
d
S
i
z
e
.
X
,
 
H
 
=
 
C
a
c
h
e
d
S
i
z
e
.
Y
;
	
f
o
r
 
(
i
n
t
3
2
 
y
 
=
 
0
;
 
y
 
<
 
H
;
 
+
+
y
)
	
{
	
	
f
o
r
 
(
i
n
t
3
2
 
x
 
=
 
0
;
 
x
 
<
 
W
;
 
+
+
x
)
	
	
{
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
i
d
 
=
 
I
d
x
(
x
,
y
,
W
)
;
	
	
	
i
f
 
(
C
a
c
h
e
d
L
a
b
e
l
s
[
i
d
]
 
!
=
 
Z
o
n
e
I
d
)
 
c
o
n
t
i
n
u
e
;
	
	
	
F
G
a
m
e
p
l
a
y
T
a
g
 
O
b
j
;
 
i
n
t
3
2
 
D
u
r
a
b
i
l
i
t
y
 
=
 
0
;
	
	
	
c
o
n
s
t
 
b
o
o
l
 
b
H
a
s
O
b
j
 
=
 
M
a
p
G
r
i
d
-
>
G
e
t
O
b
j
e
c
t
A
t
(
x
,
 
y
,
 
O
b
j
,
 
D
u
r
a
b
i
l
i
t
y
)
;
	
	
	
i
f
 
(
b
H
a
s
O
b
j
 
&
&
 
O
b
j
.
I
s
V
a
l
i
d
(
)
 
&
&
 
D
u
r
a
b
i
l
i
t
y
 
>
 
0
)
 
c
o
n
t
i
n
u
e
;
	
	
	
O
u
t
.
A
d
d
(
F
I
n
t
P
o
i
n
t
(
x
,
y
)
)
;
	
	
}
	
}
	
r
e
t
u
r
n
 
O
u
t
;
}
b
o
o
l
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
V
a
l
i
d
a
t
e
I
n
p
u
t
s
(
c
o
n
s
t
 
U
M
a
p
G
r
i
d
2
D
*
 
M
a
p
,
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
c
o
n
s
t
 
T
A
r
r
a
y
<
i
n
t
3
2
>
&
 
L
a
b
e
l
s
,
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
c
o
n
s
t
 
U
Z
o
n
e
B
o
r
d
e
r
S
e
t
t
i
n
g
s
*
 
S
e
t
t
i
n
g
s
)
 
c
o
n
s
t
{
	
i
f
 
(
!
M
a
p
 
|
|
 
!
S
e
t
t
i
n
g
s
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
S
 
=
 
M
a
p
-
>
G
e
t
S
i
z
e
(
)
;
	
i
f
 
(
S
.
X
 
<
=
 
0
 
|
|
 
S
.
Y
 
<
=
 
0
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
i
f
 
(
L
a
b
e
l
s
.
N
u
m
(
)
 
!
=
 
S
.
X
 
*
 
S
.
Y
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
i
f
 
(
!
S
e
t
t
i
n
g
s
-
>
W
a
l
l
O
b
j
e
c
t
T
a
g
.
I
s
V
a
l
i
d
(
)
 
|
|
 
S
e
t
t
i
n
g
s
-
>
W
a
l
l
D
u
r
a
b
i
l
i
t
y
 
<
=
 
0
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
i
f
 
(
S
e
t
t
i
n
g
s
-
>
B
o
r
d
e
r
T
h
i
c
k
n
e
s
s
 
<
 
1
 
|
|
 
S
e
t
t
i
n
g
s
-
>
P
a
s
s
a
g
e
W
i
d
t
h
 
<
 
1
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
r
e
t
u
r
n
 
t
r
u
e
;
}
v
o
i
d
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
C
o
l
l
e
c
t
Z
o
n
e
B
o
u
n
d
a
r
i
e
s
(
c
o
n
s
t
 
T
A
r
r
a
y
<
i
n
t
3
2
>
&
 
L
a
b
e
l
s
,
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
T
M
a
p
<
F
I
n
t
P
o
i
n
t
,
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
>
&
 
O
u
t
P
a
i
r
T
o
A
,
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
T
M
a
p
<
F
I
n
t
P
o
i
n
t
,
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
>
&
 
O
u
t
P
a
i
r
T
o
B
)
 
c
o
n
s
t
{
	
c
o
n
s
t
 
i
n
t
3
2
 
W
 
=
 
C
a
c
h
e
d
S
i
z
e
.
X
,
 
H
 
=
 
C
a
c
h
e
d
S
i
z
e
.
Y
;
	
a
u
t
o
 
P
a
i
r
K
e
y
 
=
 
[
]
(
i
n
t
3
2
 
A
,
 
i
n
t
3
2
 
B
)
-
>
F
I
n
t
P
o
i
n
t
	
{
	
	
r
e
t
u
r
n
 
(
A
 
<
 
B
)
 
?
 
F
I
n
t
P
o
i
n
t
(
A
,
B
)
 
:
 
F
I
n
t
P
o
i
n
t
(
B
,
A
)
;
	
}
;
	
f
o
r
 
(
i
n
t
3
2
 
y
 
=
 
0
;
 
y
 
<
 
H
;
 
+
+
y
)
	
{
	
	
f
o
r
 
(
i
n
t
3
2
 
x
 
=
 
0
;
 
x
 
<
 
W
;
 
+
+
x
)
	
	
{
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
z
 
=
 
L
a
b
e
l
s
[
I
d
x
(
x
,
y
,
W
)
]
;
	
	
	
i
f
 
(
z
 
<
 
0
)
 
c
o
n
t
i
n
u
e
;
	
	
	
/
/
 
R
i
g
h
t
 
n
e
i
g
h
b
o
r
	
	
	
i
f
 
(
x
 
<
 
W
 
-
 
1
)
	
	
	
{
	
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
z
2
 
=
 
L
a
b
e
l
s
[
I
d
x
(
x
+
1
,
y
,
W
)
]
;
	
	
	
	
i
f
 
(
z
2
 
>
=
 
0
 
&
&
 
z
2
 
!
=
 
z
)
	
	
	
	
{
	
	
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
K
e
y
 
=
 
P
a
i
r
K
e
y
(
z
,
 
z
2
)
;
	
	
	
	
	
/
/
 
P
u
t
 
t
h
e
 
l
e
f
t
 
c
e
l
l
 
i
n
t
o
 
t
h
e
 
l
o
w
e
r
-
i
d
 
s
i
d
e
 
s
e
t
	
	
	
	
	
i
f
 
(
K
e
y
.
X
 
=
=
 
z
)
 
 
{
 
O
u
t
P
a
i
r
T
o
A
.
F
i
n
d
O
r
A
d
d
(
K
e
y
)
.
A
d
d
(
F
I
n
t
P
o
i
n
t
(
x
,
 
 
 
y
)
)
;
 
}
	
	
	
	
	
e
l
s
e
 
 
 
 
 
 
 
 
 
 
 
 
 
{
 
O
u
t
P
a
i
r
T
o
A
.
F
i
n
d
O
r
A
d
d
(
K
e
y
)
.
A
d
d
(
F
I
n
t
P
o
i
n
t
(
x
+
1
,
 
y
)
)
;
 
}
	
	
	
	
	
/
/
 
A
n
d
 
m
i
r
r
o
r
 
t
o
 
B
 
s
i
d
e
	
	
	
	
	
i
f
 
(
K
e
y
.
Y
 
=
=
 
z
)
 
 
{
 
O
u
t
P
a
i
r
T
o
B
.
F
i
n
d
O
r
A
d
d
(
K
e
y
)
.
A
d
d
(
F
I
n
t
P
o
i
n
t
(
x
,
 
 
 
y
)
)
;
 
}
	
	
	
	
	
e
l
s
e
 
 
 
 
 
 
 
 
 
 
 
 
 
{
 
O
u
t
P
a
i
r
T
o
B
.
F
i
n
d
O
r
A
d
d
(
K
e
y
)
.
A
d
d
(
F
I
n
t
P
o
i
n
t
(
x
+
1
,
 
y
)
)
;
 
}
	
	
	
	
}
	
	
	
}
	
	
	
/
/
 
U
p
 
n
e
i
g
h
b
o
r
	
	
	
i
f
 
(
y
 
<
 
H
 
-
 
1
)
	
	
	
{
	
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
z
2
 
=
 
L
a
b
e
l
s
[
I
d
x
(
x
,
y
+
1
,
W
)
]
;
	
	
	
	
i
f
 
(
z
2
 
>
=
 
0
 
&
&
 
z
2
 
!
=
 
z
)
	
	
	
	
{
	
	
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
K
e
y
 
=
 
P
a
i
r
K
e
y
(
z
,
 
z
2
)
;
	
	
	
	
	
i
f
 
(
K
e
y
.
X
 
=
=
 
z
)
 
 
{
 
O
u
t
P
a
i
r
T
o
A
.
F
i
n
d
O
r
A
d
d
(
K
e
y
)
.
A
d
d
(
F
I
n
t
P
o
i
n
t
(
x
,
 
y
 
 
)
)
;
 
}
	
	
	
	
	
e
l
s
e
 
 
 
 
 
 
 
 
 
 
 
 
 
{
 
O
u
t
P
a
i
r
T
o
A
.
F
i
n
d
O
r
A
d
d
(
K
e
y
)
.
A
d
d
(
F
I
n
t
P
o
i
n
t
(
x
,
 
y
+
1
)
)
;
 
}
	
	
	
	
	
i
f
 
(
K
e
y
.
Y
 
=
=
 
z
)
 
 
{
 
O
u
t
P
a
i
r
T
o
B
.
F
i
n
d
O
r
A
d
d
(
K
e
y
)
.
A
d
d
(
F
I
n
t
P
o
i
n
t
(
x
,
 
y
 
 
)
)
;
 
}
	
	
	
	
	
e
l
s
e
 
 
 
 
 
 
 
 
 
 
 
 
 
{
 
O
u
t
P
a
i
r
T
o
B
.
F
i
n
d
O
r
A
d
d
(
K
e
y
)
.
A
d
d
(
F
I
n
t
P
o
i
n
t
(
x
,
 
y
+
1
)
)
;
 
}
	
	
	
	
}
	
	
	
}
	
	
}
	
}
}
v
o
i
d
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
C
h
o
o
s
e
A
n
d
C
a
r
v
e
P
a
s
s
a
g
e
s
(
	
U
M
a
p
G
r
i
d
2
D
*
 
M
a
p
,
	
c
o
n
s
t
 
U
Z
o
n
e
B
o
r
d
e
r
S
e
t
t
i
n
g
s
*
 
S
e
t
t
i
n
g
s
,
	
c
o
n
s
t
 
T
M
a
p
<
F
I
n
t
P
o
i
n
t
,
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
>
&
 
P
a
i
r
T
o
A
,
	
c
o
n
s
t
 
T
M
a
p
<
F
I
n
t
P
o
i
n
t
,
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
>
&
 
P
a
i
r
T
o
B
)
{
	
c
o
n
s
t
 
i
n
t
3
2
 
S
e
e
d
 
=
 
(
S
e
t
t
i
n
g
s
-
>
R
a
n
d
o
m
S
e
e
d
 
>
=
 
0
)
 
?
 
S
e
t
t
i
n
g
s
-
>
R
a
n
d
o
m
S
e
e
d
 
:
 
F
M
a
t
h
:
:
R
a
n
d
(
)
;
	
F
R
a
n
d
o
m
S
t
r
e
a
m
 
R
N
G
(
S
e
e
d
)
;
	
P
a
s
s
a
g
e
M
a
s
k
.
R
e
s
e
t
(
)
;
	
P
a
s
s
a
g
e
s
.
R
e
s
e
t
(
)
;
	
/
/
 
D
e
g
r
e
e
 
c
a
p
s
	
T
M
a
p
<
i
n
t
3
2
,
i
n
t
3
2
>
 
D
e
g
r
e
e
C
a
p
,
 
D
e
g
r
e
e
N
o
w
;
	
T
S
e
t
<
i
n
t
3
2
>
 
Z
o
n
e
s
I
n
v
o
l
v
e
d
;
	
f
o
r
 
(
c
o
n
s
t
 
a
u
t
o
&
 
k
v
 
:
 
P
a
i
r
T
o
A
)
 
{
 
Z
o
n
e
s
I
n
v
o
l
v
e
d
.
A
d
d
(
k
v
.
K
e
y
.
X
)
;
 
Z
o
n
e
s
I
n
v
o
l
v
e
d
.
A
d
d
(
k
v
.
K
e
y
.
Y
)
;
 
}
	
f
o
r
 
(
i
n
t
3
2
 
z
 
:
 
Z
o
n
e
s
I
n
v
o
l
v
e
d
)
 
{
 
D
e
g
r
e
e
N
o
w
.
A
d
d
(
z
,
 
0
)
;
 
D
e
g
r
e
e
C
a
p
.
A
d
d
(
z
,
 
-
1
)
;
 
}
	
f
o
r
 
(
c
o
n
s
t
 
F
Z
o
n
e
P
a
s
s
a
g
e
C
a
p
&
 
c
a
p
 
:
 
S
e
t
t
i
n
g
s
-
>
D
e
g
r
e
e
C
a
p
s
)
	
{
	
	
D
e
g
r
e
e
C
a
p
.
F
i
n
d
O
r
A
d
d
(
c
a
p
.
Z
o
n
e
I
n
d
e
x
)
 
=
 
c
a
p
.
M
a
x
P
a
s
s
a
g
e
s
;
	
	
i
f
 
(
!
D
e
g
r
e
e
N
o
w
.
C
o
n
t
a
i
n
s
(
c
a
p
.
Z
o
n
e
I
n
d
e
x
)
)
 
D
e
g
r
e
e
N
o
w
.
A
d
d
(
c
a
p
.
Z
o
n
e
I
n
d
e
x
,
 
0
)
;
	
}
	
a
u
t
o
 
Z
o
n
e
B
e
l
o
w
C
a
p
 
=
 
[
&
]
(
i
n
t
3
2
 
Z
)
-
>
b
o
o
l
{
	
	
c
o
n
s
t
 
i
n
t
3
2
 
c
a
p
 
=
 
D
e
g
r
e
e
C
a
p
.
C
o
n
t
a
i
n
s
(
Z
)
 
?
 
D
e
g
r
e
e
C
a
p
[
Z
]
 
:
 
-
1
;
	
	
c
o
n
s
t
 
i
n
t
3
2
 
d
e
g
 
=
 
D
e
g
r
e
e
N
o
w
.
C
o
n
t
a
i
n
s
(
Z
)
 
?
 
D
e
g
r
e
e
N
o
w
[
Z
]
 
:
 
0
;
	
	
r
e
t
u
r
n
 
(
c
a
p
 
<
 
0
)
 
|
|
 
(
d
e
g
 
<
 
c
a
p
)
;
	
}
;
	
/
/
 
R
a
n
d
o
m
i
z
e
 
p
a
i
r
 
o
r
d
e
r
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
P
a
i
r
s
;
 
P
a
i
r
s
.
R
e
s
e
r
v
e
(
P
a
i
r
T
o
A
.
N
u
m
(
)
)
;
	
f
o
r
 
(
c
o
n
s
t
 
a
u
t
o
&
 
k
v
 
:
 
P
a
i
r
T
o
A
)
 
P
a
i
r
s
.
A
d
d
(
k
v
.
K
e
y
)
;
	
A
l
g
o
:
:
R
a
n
d
o
m
S
h
u
f
f
l
e
(
P
a
i
r
s
)
;
	
c
o
n
s
t
 
i
n
t
3
2
 
W
 
=
 
C
a
c
h
e
d
S
i
z
e
.
X
,
 
H
 
=
 
C
a
c
h
e
d
S
i
z
e
.
Y
;
	
a
u
t
o
 
I
n
B
o
u
n
d
s
2
 
=
 
[
&
]
(
i
n
t
 
x
,
i
n
t
 
y
)
{
 
r
e
t
u
r
n
 
x
>
=
0
 
&
&
 
y
>
=
0
 
&
&
 
x
<
W
 
&
&
 
y
<
H
;
 
}
;
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
K
e
y
 
:
 
P
a
i
r
s
)
 
/
/
 
(
Z
o
n
e
A
=
m
i
n
,
 
Z
o
n
e
B
=
m
a
x
)
	
{
	
	
c
o
n
s
t
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
*
 
A
S
e
t
P
t
r
 
=
 
P
a
i
r
T
o
A
.
F
i
n
d
(
K
e
y
)
;
	
	
c
o
n
s
t
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
*
 
B
S
e
t
P
t
r
 
=
 
P
a
i
r
T
o
B
.
F
i
n
d
(
K
e
y
)
;
	
	
i
f
 
(
!
A
S
e
t
P
t
r
 
|
|
 
!
B
S
e
t
P
t
r
 
|
|
 
A
S
e
t
P
t
r
-
>
N
u
m
(
)
=
=
0
 
|
|
 
B
S
e
t
P
t
r
-
>
N
u
m
(
)
=
=
0
)
 
c
o
n
t
i
n
u
e
;
	
	
i
f
 
(
!
Z
o
n
e
B
e
l
o
w
C
a
p
(
K
e
y
.
X
)
 
|
|
 
!
Z
o
n
e
B
e
l
o
w
C
a
p
(
K
e
y
.
Y
)
)
 
c
o
n
t
i
n
u
e
;
	
	
/
/
 
C
a
n
d
i
d
a
t
e
 
a
n
c
h
o
r
s
 
o
n
 
A
 
s
i
d
e
	
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
A
C
a
n
d
s
;
	
	
A
C
a
n
d
s
.
R
e
s
e
r
v
e
(
A
S
e
t
P
t
r
-
>
N
u
m
(
)
)
;
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
c
 
:
 
*
A
S
e
t
P
t
r
)
 
A
C
a
n
d
s
.
A
d
d
(
c
)
;
	
	
A
l
g
o
:
:
R
a
n
d
o
m
S
h
u
f
f
l
e
(
A
C
a
n
d
s
)
;
	
	
b
o
o
l
 
b
C
a
r
v
e
d
 
=
 
f
a
l
s
e
;
	
	
i
n
t
3
2
 
a
t
t
e
m
p
t
s
L
e
f
t
 
=
 
F
M
a
t
h
:
:
M
a
x
(
1
,
 
S
e
t
t
i
n
g
s
-
>
A
t
t
e
m
p
t
s
P
e
r
P
a
i
r
)
;
	
	
/
/
 
T
r
y
 
u
p
 
t
o
 
A
t
t
e
m
p
t
s
P
e
r
P
a
i
r
 
d
i
f
f
e
r
e
n
t
 
a
n
c
h
o
r
s
	
	
f
o
r
 
(
i
n
t
3
2
 
a
t
t
e
m
p
t
 
=
 
0
;
 
a
t
t
e
m
p
t
 
<
 
a
t
t
e
m
p
t
s
L
e
f
t
 
&
&
 
!
b
C
a
r
v
e
d
;
 
+
+
a
t
t
e
m
p
t
)
	
	
{
	
	
	
i
f
 
(
A
C
a
n
d
s
.
N
u
m
(
)
 
=
=
 
0
)
 
b
r
e
a
k
;
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
p
i
c
k
 
=
 
R
N
G
.
R
a
n
d
R
a
n
g
e
(
0
,
 
A
C
a
n
d
s
.
N
u
m
(
)
-
1
)
;
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
A
n
c
h
o
r
A
 
=
 
A
C
a
n
d
s
[
p
i
c
k
]
;
	
	
	
A
C
a
n
d
s
.
R
e
m
o
v
e
A
t
S
w
a
p
(
p
i
c
k
)
;
	
	
	
/
/
 
D
e
t
e
r
m
i
n
e
 
b
o
u
n
d
a
r
y
 
o
r
i
e
n
t
a
t
i
o
n
 
b
y
 
c
h
e
c
k
i
n
g
 
B
 
n
e
i
g
h
b
o
r
s
 
a
r
o
u
n
d
 
t
h
e
 
a
n
c
h
o
r
	
	
	
b
o
o
l
 
b
V
e
r
t
i
c
a
l
=
f
a
l
s
e
;
 
i
n
t
 
d
x
T
o
B
=
0
,
 
d
y
T
o
B
=
0
;
	
	
	
i
f
 
(
B
S
e
t
P
t
r
-
>
C
o
n
t
a
i
n
s
(
F
I
n
t
P
o
i
n
t
(
A
n
c
h
o
r
A
.
X
+
1
,
A
n
c
h
o
r
A
.
Y
)
)
)
 
{
 
d
x
T
o
B
=
+
1
;
 
d
y
T
o
B
=
0
;
 
b
V
e
r
t
i
c
a
l
=
t
r
u
e
;
 
}
	
	
	
e
l
s
e
 
i
f
 
(
B
S
e
t
P
t
r
-
>
C
o
n
t
a
i
n
s
(
F
I
n
t
P
o
i
n
t
(
A
n
c
h
o
r
A
.
X
-
1
,
A
n
c
h
o
r
A
.
Y
)
)
)
 
{
 
d
x
T
o
B
=
-
1
;
 
d
y
T
o
B
=
0
;
 
b
V
e
r
t
i
c
a
l
=
t
r
u
e
;
 
}
	
	
	
e
l
s
e
 
i
f
 
(
B
S
e
t
P
t
r
-
>
C
o
n
t
a
i
n
s
(
F
I
n
t
P
o
i
n
t
(
A
n
c
h
o
r
A
.
X
,
A
n
c
h
o
r
A
.
Y
+
1
)
)
)
 
{
 
d
x
T
o
B
=
0
;
 
d
y
T
o
B
=
+
1
;
 
b
V
e
r
t
i
c
a
l
=
f
a
l
s
e
;
 
}
	
	
	
e
l
s
e
 
i
f
 
(
B
S
e
t
P
t
r
-
>
C
o
n
t
a
i
n
s
(
F
I
n
t
P
o
i
n
t
(
A
n
c
h
o
r
A
.
X
,
A
n
c
h
o
r
A
.
Y
-
1
)
)
)
 
{
 
d
x
T
o
B
=
0
;
 
d
y
T
o
B
=
-
1
;
 
b
V
e
r
t
i
c
a
l
=
f
a
l
s
e
;
 
}
	
	
	
e
l
s
e
	
	
	
{
	
	
	
	
/
/
 
F
a
l
l
b
a
c
k
:
 
p
i
c
k
 
a
n
y
 
B
 
c
e
l
l
 
a
n
d
 
i
n
f
e
r
 
d
i
r
e
c
t
i
o
n
	
	
	
	
c
o
n
s
t
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
&
 
B
S
e
t
 
=
 
*
B
S
e
t
P
t
r
;
	
	
	
	
i
n
t
3
2
 
j
=
0
,
 
i
d
x
 
=
 
R
N
G
.
R
a
n
d
R
a
n
g
e
(
0
,
 
B
S
e
t
.
N
u
m
(
)
-
1
)
;
	
	
	
	
F
I
n
t
P
o
i
n
t
 
a
n
y
B
 
=
 
A
n
c
h
o
r
A
;
	
	
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
b
 
:
 
B
S
e
t
)
 
{
 
i
f
 
(
j
+
+
=
=
i
d
x
)
{
 
a
n
y
B
=
b
;
 
b
r
e
a
k
;
 
}
 
}
	
	
	
	
d
x
T
o
B
 
=
 
F
M
a
t
h
:
:
C
l
a
m
p
(
a
n
y
B
.
X
-
A
n
c
h
o
r
A
.
X
,
-
1
,
1
)
;
	
	
	
	
d
y
T
o
B
 
=
 
F
M
a
t
h
:
:
C
l
a
m
p
(
a
n
y
B
.
Y
-
A
n
c
h
o
r
A
.
Y
,
-
1
,
1
)
;
	
	
	
	
b
V
e
r
t
i
c
a
l
 
=
 
(
d
x
T
o
B
!
=
0
)
;
	
	
	
}
	
	
	
/
/
 
P
r
e
v
i
e
w
 
s
t
r
i
p
e
 
t
o
 
c
a
r
v
e
 
(
n
o
 
m
a
p
 
m
o
d
i
f
i
c
a
t
i
o
n
 
y
e
t
)
	
	
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
T
o
C
l
e
a
r
A
,
 
T
o
C
l
e
a
r
B
;
	
	
	
i
f
 
(
!
B
u
i
l
d
C
a
r
v
e
S
t
r
i
p
e
P
r
e
v
i
e
w
(
M
a
p
,
 
A
n
c
h
o
r
A
,
 
b
V
e
r
t
i
c
a
l
,
 
d
x
T
o
B
,
 
d
y
T
o
B
,
	
	
	
	
S
e
t
t
i
n
g
s
-
>
P
a
s
s
a
g
e
W
i
d
t
h
,
 
K
e
y
.
X
,
 
K
e
y
.
Y
,
 
T
o
C
l
e
a
r
A
,
 
T
o
C
l
e
a
r
B
)
)
	
	
	
{
	
	
	
	
c
o
n
t
i
n
u
e
;
 
/
/
 
i
n
v
a
l
i
d
 
a
n
c
h
o
r
:
 
w
o
u
l
d
 
e
x
i
t
 
b
o
u
n
d
s
 
o
r
 
h
i
t
 
w
r
o
n
g
 
z
o
n
e
s
	
	
	
}
	
	
	
/
/
 
S
p
a
c
i
n
g
 
c
h
e
c
k
 
(
g
l
o
b
a
l
)
	
	
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
P
r
e
v
i
e
w
;
	
	
	
P
r
e
v
i
e
w
.
R
e
s
e
r
v
e
(
T
o
C
l
e
a
r
A
.
N
u
m
(
)
 
+
 
T
o
C
l
e
a
r
B
.
N
u
m
(
)
)
;
	
	
	
P
r
e
v
i
e
w
.
A
p
p
e
n
d
(
T
o
C
l
e
a
r
A
)
;
 
P
r
e
v
i
e
w
.
A
p
p
e
n
d
(
T
o
C
l
e
a
r
B
)
;
	
	
	
i
f
 
(
I
s
T
o
o
C
l
o
s
e
T
o
E
x
i
s
t
i
n
g
P
a
s
s
a
g
e
s
(
P
r
e
v
i
e
w
,
 
S
e
t
t
i
n
g
s
-
>
M
i
n
P
a
s
s
a
g
e
D
i
s
t
a
n
c
e
)
)
	
	
	
{
	
	
	
	
c
o
n
t
i
n
u
e
;
 
/
/
 
t
o
o
 
c
l
o
s
e
 
t
o
 
e
x
i
s
t
i
n
g
 
p
a
s
s
a
g
e
s
	
	
	
}
	
	
	
/
/
 
C
a
r
v
e
 
f
o
r
 
r
e
a
l
	
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
p
 
:
 
T
o
C
l
e
a
r
A
)
 
C
l
e
a
r
C
e
l
l
(
M
a
p
,
 
p
.
X
,
 
p
.
Y
)
;
	
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
p
 
:
 
T
o
C
l
e
a
r
B
)
 
C
l
e
a
r
C
e
l
l
(
M
a
p
,
 
p
.
X
,
 
p
.
Y
)
;
	
	
	
/
/
 
R
e
c
o
r
d
 
p
a
s
s
a
g
e
	
	
	
F
Z
o
n
e
P
a
s
s
a
g
e
 
P
a
s
s
;
 
	
	
	
P
a
s
s
.
Z
o
n
e
A
 
=
 
K
e
y
.
X
;
 
P
a
s
s
.
Z
o
n
e
B
 
=
 
K
e
y
.
Y
;
	
	
	
P
a
s
s
.
C
e
l
l
s
 
=
 
M
o
v
e
T
e
m
p
(
P
r
e
v
i
e
w
)
;
	
	
	
P
a
s
s
a
g
e
s
.
A
d
d
(
M
o
v
e
T
e
m
p
(
P
a
s
s
)
)
;
	
	
	
D
e
g
r
e
e
N
o
w
[
K
e
y
.
X
]
+
+
;
 
D
e
g
r
e
e
N
o
w
[
K
e
y
.
Y
]
+
+
;
	
	
	
/
/
 
A
s
s
i
g
n
 
e
a
c
h
 
p
a
s
s
a
g
e
 
c
e
l
l
 
t
o
 
t
h
e
 
a
d
j
a
c
e
n
t
 
m
a
j
o
r
i
t
y
 
z
o
n
e
 
(
A
 
o
r
 
B
)
 
b
a
s
e
d
 
o
n
 
4
-
n
e
i
g
h
b
o
r
s
	
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
C
e
l
l
 
:
 
P
a
s
s
a
g
e
s
.
L
a
s
t
(
)
.
C
e
l
l
s
)
	
	
	
{
	
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
l
i
d
 
=
 
I
d
x
(
C
e
l
l
.
X
,
 
C
e
l
l
.
Y
,
 
C
a
c
h
e
d
S
i
z
e
.
X
)
;
	
	
	
	
i
f
 
(
l
i
d
 
<
 
0
 
|
|
 
l
i
d
 
>
=
 
C
a
c
h
e
d
L
a
b
e
l
s
.
N
u
m
(
)
)
 
c
o
n
t
i
n
u
e
;
	
	
	
	
i
n
t
3
2
 
c
o
u
n
t
A
 
=
 
0
,
 
c
o
u
n
t
B
 
=
 
0
;
	
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
N
4
[
4
]
 
=
 
{
 
F
I
n
t
P
o
i
n
t
(
C
e
l
l
.
X
+
1
,
C
e
l
l
.
Y
)
,
 
F
I
n
t
P
o
i
n
t
(
C
e
l
l
.
X
-
1
,
C
e
l
l
.
Y
)
,
 
F
I
n
t
P
o
i
n
t
(
C
e
l
l
.
X
,
C
e
l
l
.
Y
+
1
)
,
 
F
I
n
t
P
o
i
n
t
(
C
e
l
l
.
X
,
C
e
l
l
.
Y
-
1
)
 
}
;
	
	
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
n
 
:
 
N
4
)
	
	
	
	
{
	
	
	
	
	
i
f
 
(
!
I
n
B
o
u
n
d
s
(
n
.
X
,
 
n
.
Y
)
)
 
c
o
n
t
i
n
u
e
;
	
	
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
n
i
d
 
=
 
I
d
x
(
n
.
X
,
 
n
.
Y
,
 
C
a
c
h
e
d
S
i
z
e
.
X
)
;
	
	
	
	
	
i
f
 
(
n
i
d
 
<
 
0
 
|
|
 
n
i
d
 
>
=
 
C
a
c
h
e
d
L
a
b
e
l
s
.
N
u
m
(
)
)
 
c
o
n
t
i
n
u
e
;
	
	
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
z
 
=
 
C
a
c
h
e
d
L
a
b
e
l
s
[
n
i
d
]
;
	
	
	
	
	
i
f
 
(
z
 
=
=
 
K
e
y
.
X
)
 
+
+
c
o
u
n
t
A
;
 
e
l
s
e
 
i
f
 
(
z
 
=
=
 
K
e
y
.
Y
)
 
+
+
c
o
u
n
t
B
;
	
	
	
	
}
	
	
	
	
i
n
t
3
2
 
t
a
r
g
e
t
 
=
 
C
a
c
h
e
d
L
a
b
e
l
s
[
l
i
d
]
;
	
	
	
	
i
f
 
(
c
o
u
n
t
A
 
>
 
c
o
u
n
t
B
)
 
t
a
r
g
e
t
 
=
 
K
e
y
.
X
;
 
e
l
s
e
 
i
f
 
(
c
o
u
n
t
B
 
>
 
c
o
u
n
t
A
)
 
t
a
r
g
e
t
 
=
 
K
e
y
.
Y
;
 
/
/
 
t
i
e
:
 
k
e
e
p
 
c
u
r
r
e
n
t
	
	
	
	
i
f
 
(
t
a
r
g
e
t
 
!
=
 
C
a
c
h
e
d
L
a
b
e
l
s
[
l
i
d
]
)
	
	
	
	
{
	
	
	
	
	
C
a
c
h
e
d
L
a
b
e
l
s
[
l
i
d
]
 
=
 
t
a
r
g
e
t
;
	
	
	
	
	
M
a
p
-
>
S
e
t
Z
o
n
e
A
t
(
C
e
l
l
.
X
,
 
C
e
l
l
.
Y
,
 
t
a
r
g
e
t
)
;
	
	
	
	
}
	
	
	
}
	
	
	
/
/
 
U
p
d
a
t
e
 
p
r
o
t
e
c
t
i
o
n
 
m
a
s
k
:
 
p
a
s
s
a
g
e
 
c
e
l
l
s
 
+
 
d
i
l
a
t
i
o
n
 
t
o
 
p
r
o
t
e
c
t
 
f
r
o
m
 
a
n
y
 
w
a
l
l
s
 
&
 
e
n
f
o
r
c
e
 
s
p
a
c
i
n
g
	
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
c
 
:
 
P
a
s
s
a
g
e
s
.
L
a
s
t
(
)
.
C
e
l
l
s
)
 
P
a
s
s
a
g
e
M
a
s
k
.
A
d
d
(
c
)
;
	
	
	
D
i
l
a
t
e
M
a
s
k
(
P
a
s
s
a
g
e
M
a
s
k
,
 
F
M
a
t
h
:
:
M
a
x
(
0
,
 
S
e
t
t
i
n
g
s
-
>
B
o
r
d
e
r
T
h
i
c
k
n
e
s
s
 
-
 
1
)
)
;
 
/
/
 
s
h
i
e
l
d
 
f
r
o
m
 
t
h
i
c
k
 
b
o
r
d
e
r
s
	
	
	
D
i
l
a
t
e
M
a
s
k
(
P
a
s
s
a
g
e
M
a
s
k
,
 
S
e
t
t
i
n
g
s
-
>
M
i
n
P
a
s
s
a
g
e
D
i
s
t
a
n
c
e
)
;
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
/
/
 
s
p
a
c
i
n
g
 
f
o
r
 
f
u
t
u
r
e
 
p
a
s
s
a
g
e
s
	
	
	
/
/
 
D
e
b
u
g
:
 
c
o
l
o
r
 
b
y
 
z
o
n
e
 
s
o
 
e
n
d
s
 
d
i
f
f
e
r
	
	
	
i
f
 
(
S
e
t
t
i
n
g
s
-
>
b
D
e
b
u
g
D
r
a
w
P
a
s
s
a
g
e
s
)
	
	
	
{
	
	
	
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
C
e
l
l
s
A
;
 
C
e
l
l
s
A
.
R
e
s
e
r
v
e
(
P
a
s
s
a
g
e
s
.
L
a
s
t
(
)
.
C
e
l
l
s
.
N
u
m
(
)
)
;
	
	
	
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
C
e
l
l
s
B
;
 
C
e
l
l
s
B
.
R
e
s
e
r
v
e
(
P
a
s
s
a
g
e
s
.
L
a
s
t
(
)
.
C
e
l
l
s
.
N
u
m
(
)
)
;
	
	
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
c
 
:
 
P
a
s
s
a
g
e
s
.
L
a
s
t
(
)
.
C
e
l
l
s
)
	
	
	
	
{
	
	
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
i
d
 
=
 
I
d
x
(
c
.
X
,
 
c
.
Y
,
 
C
a
c
h
e
d
S
i
z
e
.
X
)
;
	
	
	
	
	
i
f
 
(
i
d
 
>
=
 
0
 
&
&
 
i
d
 
<
 
C
a
c
h
e
d
L
a
b
e
l
s
.
N
u
m
(
)
)
	
	
	
	
	
{
	
	
	
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
z
 
=
 
C
a
c
h
e
d
L
a
b
e
l
s
[
i
d
]
;
	
	
	
	
	
	
i
f
 
(
z
 
=
=
 
K
e
y
.
X
)
 
C
e
l
l
s
A
.
A
d
d
(
c
)
;
	
	
	
	
	
	
e
l
s
e
 
i
f
 
(
z
 
=
=
 
K
e
y
.
Y
)
 
C
e
l
l
s
B
.
A
d
d
(
c
)
;
	
	
	
	
	
}
	
	
	
	
}
	
	
	
	
i
f
 
(
C
e
l
l
s
A
.
N
u
m
(
)
 
>
 
0
)
 
D
e
b
u
g
D
r
a
w
P
a
s
s
a
g
e
C
e
l
l
s
(
S
e
t
t
i
n
g
s
,
 
C
e
l
l
s
A
,
 
F
C
o
l
o
r
:
:
R
e
d
,
 
M
a
p
-
>
G
e
t
W
o
r
l
d
(
)
)
;
	
	
	
	
i
f
 
(
C
e
l
l
s
B
.
N
u
m
(
)
 
>
 
0
)
 
D
e
b
u
g
D
r
a
w
P
a
s
s
a
g
e
C
e
l
l
s
(
S
e
t
t
i
n
g
s
,
 
C
e
l
l
s
B
,
 
F
C
o
l
o
r
:
:
B
l
u
e
,
 
M
a
p
-
>
G
e
t
W
o
r
l
d
(
)
)
;
	
	
	
}
	
	
	
b
C
a
r
v
e
d
 
=
 
t
r
u
e
;
	
	
}
	
}
}
v
o
i
d
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
P
l
a
c
e
W
a
l
l
s
W
i
t
h
T
h
i
c
k
n
e
s
s
(
U
M
a
p
G
r
i
d
2
D
*
 
M
a
p
,
	
	
	
	
	
	
	
	
	
	
	
	
 
 
 
c
o
n
s
t
 
U
Z
o
n
e
B
o
r
d
e
r
S
e
t
t
i
n
g
s
*
 
S
e
t
t
i
n
g
s
,
	
	
	
	
	
	
	
	
	
	
	
	
 
 
 
c
o
n
s
t
 
T
M
a
p
<
F
I
n
t
P
o
i
n
t
,
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
>
&
 
P
a
i
r
T
o
A
)
{
	
/
/
 
P
a
s
s
a
g
e
M
a
s
k
 
a
l
r
e
a
d
y
 
c
o
n
t
a
i
n
s
 
t
h
e
 
c
a
r
v
e
d
 
c
e
l
l
s
 
p
l
u
s
 
p
r
o
t
e
c
t
i
v
e
 
d
i
l
a
t
i
o
n
.
	
f
o
r
 
(
c
o
n
s
t
 
a
u
t
o
&
 
k
v
 
:
 
P
a
i
r
T
o
A
)
	
{
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
K
e
y
 
=
 
k
v
.
K
e
y
;
 
/
/
 
(
Z
o
n
e
A
,
 
Z
o
n
e
B
)
	
	
c
o
n
s
t
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
&
 
A
S
e
e
d
s
 
=
 
k
v
.
V
a
l
u
e
;
	
	
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
 
A
E
x
p
a
n
d
e
d
;
	
	
E
x
p
a
n
d
I
n
w
a
r
d
W
i
t
h
i
n
Z
o
n
e
(
A
S
e
e
d
s
,
 
K
e
y
.
X
,
 
S
e
t
t
i
n
g
s
-
>
B
o
r
d
e
r
T
h
i
c
k
n
e
s
s
 
-
 
1
,
 
A
E
x
p
a
n
d
e
d
)
;
	
	
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
 
T
o
W
a
l
l
 
=
 
A
S
e
e
d
s
;
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
p
 
:
 
A
E
x
p
a
n
d
e
d
)
 
T
o
W
a
l
l
.
A
d
d
(
p
)
;
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
p
 
:
 
T
o
W
a
l
l
)
	
	
{
	
	
	
i
f
 
(
P
a
s
s
a
g
e
M
a
s
k
.
C
o
n
t
a
i
n
s
(
p
)
)
 
c
o
n
t
i
n
u
e
;
 
/
/
 
k
e
e
p
 
p
a
s
s
a
g
e
 
&
 
b
u
f
f
e
r
 
f
r
e
e
	
	
	
P
u
t
W
a
l
l
(
M
a
p
,
 
p
.
X
,
 
p
.
Y
,
 
S
e
t
t
i
n
g
s
)
;
	
	
}
	
}
}
v
o
i
d
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
E
x
p
a
n
d
I
n
w
a
r
d
W
i
t
h
i
n
Z
o
n
e
(
c
o
n
s
t
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
&
 
S
e
e
d
s
,
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
i
n
t
3
2
 
Z
o
n
e
I
d
,
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
i
n
t
3
2
 
R
i
n
g
s
,
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
&
 
O
u
t
E
x
p
a
n
d
e
d
)
 
c
o
n
s
t
{
	
i
f
 
(
R
i
n
g
s
 
<
=
 
0
 
|
|
 
S
e
e
d
s
.
N
u
m
(
)
 
=
=
 
0
)
 
r
e
t
u
r
n
;
	
c
o
n
s
t
 
i
n
t
3
2
 
W
 
=
 
C
a
c
h
e
d
S
i
z
e
.
X
,
 
H
 
=
 
C
a
c
h
e
d
S
i
z
e
.
Y
;
	
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
 
C
u
r
r
e
n
t
 
=
 
S
e
e
d
s
;
	
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
 
V
i
s
i
t
e
d
 
=
 
S
e
e
d
s
;
	
f
o
r
 
(
i
n
t
3
2
 
r
 
=
 
0
;
 
r
 
<
 
R
i
n
g
s
;
 
+
+
r
)
	
{
	
	
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
 
N
e
x
t
;
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
c
 
:
 
C
u
r
r
e
n
t
)
	
	
{
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
N
4
[
4
]
 
=
 
{
 
{
c
.
X
+
1
,
c
.
Y
}
,
{
c
.
X
-
1
,
c
.
Y
}
,
{
c
.
X
,
c
.
Y
+
1
}
,
{
c
.
X
,
c
.
Y
-
1
}
 
}
;
	
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
n
 
:
 
N
4
)
	
	
	
{
	
	
	
	
i
f
 
(
n
.
X
 
<
 
0
 
|
|
 
n
.
Y
 
<
 
0
 
|
|
 
n
.
X
 
>
=
 
W
 
|
|
 
n
.
Y
 
>
=
 
H
)
 
c
o
n
t
i
n
u
e
;
	
	
	
	
i
f
 
(
V
i
s
i
t
e
d
.
C
o
n
t
a
i
n
s
(
n
)
)
 
c
o
n
t
i
n
u
e
;
	
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
l
a
b
 
=
 
C
a
c
h
e
d
L
a
b
e
l
s
[
I
d
x
(
n
.
X
,
n
.
Y
,
W
)
]
;
	
	
	
	
i
f
 
(
l
a
b
 
!
=
 
Z
o
n
e
I
d
)
 
c
o
n
t
i
n
u
e
;
 
 
 
 
 
 
 
 
 
 
 
 
 
 
/
/
 
s
t
a
y
 
i
n
s
i
d
e
 
t
h
e
 
z
o
n
e
	
	
	
	
i
f
 
(
P
a
s
s
a
g
e
M
a
s
k
.
C
o
n
t
a
i
n
s
(
n
)
)
 
c
o
n
t
i
n
u
e
;
 
 
 
 
 
/
/
 
d
o
 
n
o
t
 
o
v
e
r
l
a
p
 
a
n
 
o
p
e
n
i
n
g
	
	
	
	
V
i
s
i
t
e
d
.
A
d
d
(
n
)
;
	
	
	
	
N
e
x
t
.
A
d
d
(
n
)
;
	
	
	
}
	
	
}
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
v
 
:
 
N
e
x
t
)
 
O
u
t
E
x
p
a
n
d
e
d
.
A
d
d
(
v
)
;
	
	
C
u
r
r
e
n
t
 
=
 
M
o
v
e
T
e
m
p
(
N
e
x
t
)
;
	
	
i
f
 
(
C
u
r
r
e
n
t
.
N
u
m
(
)
 
=
=
 
0
)
 
b
r
e
a
k
;
	
}
}
v
o
i
d
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
P
u
t
W
a
l
l
(
U
M
a
p
G
r
i
d
2
D
*
 
M
a
p
,
 
i
n
t
3
2
 
X
,
 
i
n
t
3
2
 
Y
,
 
c
o
n
s
t
 
U
Z
o
n
e
B
o
r
d
e
r
S
e
t
t
i
n
g
s
*
 
S
e
t
t
i
n
g
s
)
 
c
o
n
s
t
{
	
M
a
p
-
>
A
d
d
O
r
U
p
d
a
t
e
O
b
j
e
c
t
A
t
(
X
,
 
Y
,
 
S
e
t
t
i
n
g
s
-
>
W
a
l
l
O
b
j
e
c
t
T
a
g
,
 
S
e
t
t
i
n
g
s
-
>
W
a
l
l
D
u
r
a
b
i
l
i
t
y
)
;
}
v
o
i
d
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
C
l
e
a
r
C
e
l
l
(
U
M
a
p
G
r
i
d
2
D
*
 
M
a
p
,
 
i
n
t
3
2
 
X
,
 
i
n
t
3
2
 
Y
)
 
c
o
n
s
t
{
	
M
a
p
-
>
R
e
m
o
v
e
O
b
j
e
c
t
A
t
(
X
,
 
Y
)
;
}
v
o
i
d
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
D
e
b
u
g
D
r
a
w
P
a
s
s
a
g
e
C
e
l
l
s
(
c
o
n
s
t
 
U
Z
o
n
e
B
o
r
d
e
r
S
e
t
t
i
n
g
s
*
 
S
e
t
t
i
n
g
s
,
 
c
o
n
s
t
 
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
&
 
C
e
l
l
s
,
 
c
o
n
s
t
 
F
C
o
l
o
r
&
 
C
o
l
o
r
,
 
U
W
o
r
l
d
*
 
W
o
r
l
d
)
 
c
o
n
s
t
{
 
 
 
 
i
f
 
(
!
S
e
t
t
i
n
g
s
-
>
b
D
e
b
u
g
D
r
a
w
P
a
s
s
a
g
e
s
 
|
|
 
!
W
o
r
l
d
)
 
r
e
t
u
r
n
;
 
 
 
 
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
c
 
:
 
C
e
l
l
s
)
 
 
 
 
{
 
 
 
 
 
 
 
 
c
o
n
s
t
 
F
V
e
c
t
o
r
 
P
(
c
.
X
 
*
 
S
e
t
t
i
n
g
s
-
>
D
e
b
u
g
T
i
l
e
S
i
z
e
U
U
,
 
c
.
Y
 
*
 
S
e
t
t
i
n
g
s
-
>
D
e
b
u
g
T
i
l
e
S
i
z
e
U
U
,
 
S
e
t
t
i
n
g
s
-
>
D
e
b
u
g
Z
O
f
f
s
e
t
)
;
 
 
 
 
 
 
 
 
/
/
 
C
h
o
o
s
e
 
c
o
l
o
r
 
b
a
s
e
d
 
o
n
 
t
h
e
 
z
o
n
e
 
i
d
 
a
t
 
t
h
i
s
 
c
e
l
l
 
(
f
a
l
l
b
a
c
k
 
t
o
 
p
r
o
v
i
d
e
d
 
C
o
l
o
r
)
 
 
 
 
 
 
 
 
F
C
o
l
o
r
 
U
s
e
C
o
l
o
r
 
=
 
C
o
l
o
r
;
 
 
 
 
 
 
 
 
c
o
n
s
t
 
i
n
t
3
2
 
W
 
=
 
C
a
c
h
e
d
S
i
z
e
.
X
;
 
 
 
 
 
 
 
 
c
o
n
s
t
 
i
n
t
3
2
 
H
 
=
 
C
a
c
h
e
d
S
i
z
e
.
Y
;
 
 
 
 
 
 
 
 
i
f
 
(
W
 
>
 
0
 
&
&
 
H
 
>
 
0
 
&
&
 
C
a
c
h
e
d
L
a
b
e
l
s
.
N
u
m
(
)
 
=
=
 
W
 
*
 
H
 
&
&
 
c
.
X
 
>
=
 
0
 
&
&
 
c
.
Y
 
>
=
 
0
 
&
&
 
c
.
X
 
<
 
W
 
&
&
 
c
.
Y
 
<
 
H
)
 
 
 
 
 
 
 
 
{
 
 
 
 
 
 
 
 
 
 
 
 
c
o
n
s
t
 
i
n
t
3
2
 
i
d
 
=
 
I
d
x
(
c
.
X
,
 
c
.
Y
,
 
W
)
;
 
 
 
 
 
 
 
 
 
 
 
 
c
o
n
s
t
 
i
n
t
3
2
 
Z
o
n
e
I
d
 
=
 
C
a
c
h
e
d
L
a
b
e
l
s
.
I
s
V
a
l
i
d
I
n
d
e
x
(
i
d
)
 
?
 
C
a
c
h
e
d
L
a
b
e
l
s
[
i
d
]
 
:
 
-
1
;
 
 
 
 
 
 
 
 
 
 
 
 
i
f
 
(
Z
o
n
e
I
d
 
>
=
 
0
)
 
 
 
 
 
 
 
 
 
 
 
 
{
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
c
o
n
s
t
 
u
i
n
t
8
 
H
8
 
=
 
u
i
n
t
8
(
(
Z
o
n
e
I
d
 
*
 
4
7
)
 
&
 
0
x
F
F
)
;
 
/
/
 
d
e
t
e
r
m
i
n
i
s
t
i
c
 
p
e
r
-
z
o
n
e
 
h
u
e
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
c
o
n
s
t
 
F
L
i
n
e
a
r
C
o
l
o
r
 
L
i
n
 
=
 
F
L
i
n
e
a
r
C
o
l
o
r
:
:
M
a
k
e
F
r
o
m
H
S
V
8
(
H
8
,
 
2
2
0
,
 
2
5
5
)
;
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
U
s
e
C
o
l
o
r
 
=
 
L
i
n
.
T
o
F
C
o
l
o
r
(
t
r
u
e
)
;
 
 
 
 
 
 
 
 
 
 
 
 
}
 
 
 
 
 
 
 
 
}
 
 
 
 
 
 
 
 
D
r
a
w
D
e
b
u
g
S
p
h
e
r
e
(
W
o
r
l
d
,
 
P
,
 
S
e
t
t
i
n
g
s
-
>
D
e
b
u
g
S
p
h
e
r
e
R
a
d
i
u
s
U
U
,
 
1
2
,
 
U
s
e
C
o
l
o
r
,
 
S
e
t
t
i
n
g
s
-
>
D
e
b
u
g
L
i
f
e
t
i
m
e
 
<
=
 
0
.
f
,
 
S
e
t
t
i
n
g
s
-
>
D
e
b
u
g
L
i
f
e
t
i
m
e
)
;
 
 
 
 
}
}
v
o
i
d
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
D
i
l
a
t
e
M
a
s
k
(
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
&
 
I
n
O
u
t
M
a
s
k
,
 
i
n
t
3
2
 
R
a
d
i
u
s
)
 
c
o
n
s
t
{
	
i
f
 
(
R
a
d
i
u
s
 
<
=
 
0
)
 
r
e
t
u
r
n
;
	
T
S
e
t
<
F
I
n
t
P
o
i
n
t
>
 
A
d
d
;
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
p
 
:
 
I
n
O
u
t
M
a
s
k
)
	
{
	
	
f
o
r
 
(
i
n
t
3
2
 
d
y
 
=
 
-
R
a
d
i
u
s
;
 
d
y
 
<
=
 
R
a
d
i
u
s
;
 
+
+
d
y
)
	
	
{
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
r
e
m
 
=
 
R
a
d
i
u
s
 
-
 
F
M
a
t
h
:
:
A
b
s
(
d
y
)
;
	
	
	
f
o
r
 
(
i
n
t
3
2
 
d
x
 
=
 
-
r
e
m
;
 
d
x
 
<
=
 
r
e
m
;
 
+
+
d
x
)
	
	
	
{
	
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
q
(
p
.
X
 
+
 
d
x
,
 
p
.
Y
 
+
 
d
y
)
;
	
	
	
	
i
f
 
(
I
n
B
o
u
n
d
s
(
q
.
X
,
 
q
.
Y
)
)
 
A
d
d
.
A
d
d
(
q
)
;
	
	
	
}
	
	
}
	
}
	
I
n
O
u
t
M
a
s
k
.
A
p
p
e
n
d
(
A
d
d
)
;
}
b
o
o
l
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
I
s
T
o
o
C
l
o
s
e
T
o
E
x
i
s
t
i
n
g
P
a
s
s
a
g
e
s
(
c
o
n
s
t
 
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
&
 
C
a
n
d
i
d
a
t
e
C
e
l
l
s
,
 
i
n
t
3
2
 
M
i
n
D
i
s
t
)
 
c
o
n
s
t
{
	
i
f
 
(
M
i
n
D
i
s
t
 
<
=
 
0
 
|
|
 
P
a
s
s
a
g
e
M
a
s
k
.
N
u
m
(
)
 
=
=
 
0
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
c
 
:
 
C
a
n
d
i
d
a
t
e
C
e
l
l
s
)
	
{
	
	
f
o
r
 
(
i
n
t
3
2
 
d
y
 
=
 
-
M
i
n
D
i
s
t
;
 
d
y
 
<
=
 
M
i
n
D
i
s
t
;
 
+
+
d
y
)
	
	
{
	
	
	
c
o
n
s
t
 
i
n
t
3
2
 
r
e
m
 
=
 
M
i
n
D
i
s
t
 
-
 
F
M
a
t
h
:
:
A
b
s
(
d
y
)
;
	
	
	
f
o
r
 
(
i
n
t
3
2
 
d
x
 
=
 
-
r
e
m
;
 
d
x
 
<
=
 
r
e
m
;
 
+
+
d
x
)
	
	
	
{
	
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
q
(
c
.
X
 
+
 
d
x
,
 
c
.
Y
 
+
 
d
y
)
;
	
	
	
	
i
f
 
(
!
I
n
B
o
u
n
d
s
(
q
.
X
,
 
q
.
Y
)
)
 
c
o
n
t
i
n
u
e
;
	
	
	
	
i
f
 
(
P
a
s
s
a
g
e
M
a
s
k
.
C
o
n
t
a
i
n
s
(
q
)
)
 
r
e
t
u
r
n
 
t
r
u
e
;
	
	
	
}
	
	
}
	
}
	
r
e
t
u
r
n
 
f
a
l
s
e
;
}
/
/
 
A
n
y
 
b
l
o
c
k
i
n
g
 
o
b
j
e
c
t
 
=
 
o
c
c
u
p
i
e
d
b
o
o
l
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
I
s
O
c
c
u
p
i
e
d
(
U
M
a
p
G
r
i
d
2
D
*
 
M
a
p
,
 
i
n
t
3
2
 
X
,
 
i
n
t
3
2
 
Y
)
 
c
o
n
s
t
{
	
F
G
a
m
e
p
l
a
y
T
a
g
 
O
b
j
;
 
i
n
t
3
2
 
D
u
r
;
 
	
i
f
 
(
!
M
a
p
-
>
G
e
t
O
b
j
e
c
t
A
t
(
X
,
 
Y
,
 
O
b
j
,
 
D
u
r
)
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
r
e
t
u
r
n
 
O
b
j
.
I
s
V
a
l
i
d
(
)
 
&
&
 
D
u
r
 
>
 
0
;
}
/
*
*
 
*
 
B
u
i
l
d
 
a
 
p
r
e
v
i
e
w
 
o
f
 
t
h
e
 
c
e
l
l
s
 
t
o
 
c
l
e
a
r
 
f
o
r
 
a
 
p
e
r
p
e
n
d
i
c
u
l
a
r
 
p
a
s
s
a
g
e
 
o
f
 
g
i
v
e
n
 
w
i
d
t
h
.
 
*
 
W
e
 
a
l
r
e
a
d
y
 
h
a
v
e
 
w
a
l
l
s
 
p
l
a
c
e
d
.
 
F
r
o
m
 
t
h
e
 
a
n
c
h
o
r
,
 
f
o
r
 
e
a
c
h
 
c
o
l
u
m
n
 
o
f
 
t
h
e
 
s
t
r
i
p
e
:
 
*
 
 
-
 
s
t
e
p
 
i
n
w
a
r
d
 
i
n
t
o
 
Z
o
n
e
A
 
a
l
o
n
g
 
i
t
s
 
i
n
w
a
r
d
 
n
o
r
m
a
l
 
u
n
t
i
l
 
w
e
 
h
i
t
 
t
h
e
 
f
i
r
s
t
 
E
M
P
T
Y
 
c
e
l
l
 
*
*
i
n
 
Z
o
n
e
A
*
*
,
 
*
 
 
 
 
a
c
c
u
m
u
l
a
t
i
n
g
 
a
l
l
 
o
c
c
u
p
i
e
d
 
c
e
l
l
s
 
o
n
 
t
h
e
 
w
a
y
 
(
t
h
e
y
 
w
i
l
l
 
b
e
 
c
l
e
a
r
e
d
)
;
 
*
 
 
-
 
s
t
e
p
 
o
u
t
w
a
r
d
 
i
n
t
o
 
Z
o
n
e
B
 
(
a
c
r
o
s
s
 
b
o
u
n
d
a
r
y
)
 
u
n
t
i
l
 
f
i
r
s
t
 
E
M
P
T
Y
 
c
e
l
l
 
*
*
i
n
 
Z
o
n
e
B
*
*
,
 
a
c
c
u
m
u
l
a
t
i
n
g
 
o
c
c
u
p
i
e
d
 
c
e
l
l
s
.
 
*
 
I
f
 
e
i
t
h
e
r
 
s
i
d
e
 
h
i
t
s
 
a
n
 
E
M
P
T
Y
 
c
e
l
l
 
b
e
l
o
n
g
i
n
g
 
t
o
 
a
 
w
r
o
n
g
 
z
o
n
e
 
(
o
r
 
l
e
a
v
e
s
 
b
o
u
n
d
s
)
,
 
w
e
 
F
A
I
L
 
t
h
i
s
 
a
n
c
h
o
r
.
 
*
 
W
e
 
o
n
l
y
 
P
R
E
V
I
E
W
 
h
e
r
e
—
n
o
 
m
o
d
i
f
i
c
a
t
i
o
n
s
 
t
o
 
t
h
e
 
m
a
p
 
y
e
t
.
 
*
/
b
o
o
l
 
U
Z
o
n
e
B
o
r
d
e
r
G
e
n
e
r
a
t
o
r
:
:
B
u
i
l
d
C
a
r
v
e
S
t
r
i
p
e
P
r
e
v
i
e
w
(
	
U
M
a
p
G
r
i
d
2
D
*
 
M
a
p
,
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
A
n
c
h
o
r
A
,
	
b
o
o
l
 
b
V
e
r
t
i
c
a
l
B
o
u
n
d
a
r
y
,
 
 
 
 
 
 
/
/
 
t
r
u
e
 
=
>
 
b
o
u
n
d
a
r
y
 
n
o
r
m
a
l
 
i
s
 
X
;
 
p
a
s
s
a
g
e
 
t
a
n
g
e
n
t
 
i
s
 
Y
	
i
n
t
 
d
x
T
o
B
,
 
i
n
t
 
d
y
T
o
B
,
 
 
 
 
 
 
 
 
/
/
 
u
n
i
t
 
v
e
c
t
o
r
 
f
r
o
m
 
A
 
t
o
 
B
 
a
c
r
o
s
s
 
t
h
e
 
b
o
u
n
d
a
r
y
 
(
±
1
,
0
)
 
o
r
 
(
0
,
±
1
)
	
i
n
t
3
2
 
P
a
s
s
a
g
e
W
i
d
t
h
,
	
i
n
t
3
2
 
Z
o
n
e
A
,
 
i
n
t
3
2
 
Z
o
n
e
B
,
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
&
 
O
u
t
C
e
l
l
s
A
,
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
&
 
O
u
t
C
e
l
l
s
B
)
 
c
o
n
s
t
{
	
O
u
t
C
e
l
l
s
A
.
R
e
s
e
t
(
)
;
	
O
u
t
C
e
l
l
s
B
.
R
e
s
e
t
(
)
;
	
c
o
n
s
t
 
i
n
t
3
2
 
W
 
=
 
C
a
c
h
e
d
S
i
z
e
.
X
,
 
H
 
=
 
C
a
c
h
e
d
S
i
z
e
.
Y
;
	
a
u
t
o
 
I
n
B
o
u
n
d
s
2
 
=
 
[
&
]
(
i
n
t
 
x
,
 
i
n
t
 
y
)
{
 
r
e
t
u
r
n
 
x
>
=
0
 
&
&
 
y
>
=
0
 
&
&
 
x
<
W
 
&
&
 
y
<
H
;
 
}
;
	
/
/
 
H
e
l
p
e
r
:
 
c
h
e
c
k
 
i
f
 
a
 
c
e
l
l
 
h
a
s
 
n
o
 
b
l
o
c
k
i
n
g
 
o
b
j
e
c
t
	
a
u
t
o
 
I
s
E
m
p
t
y
 
=
 
[
&
]
(
i
n
t
 
x
,
 
i
n
t
 
y
)
-
>
b
o
o
l
	
{
	
	
F
G
a
m
e
p
l
a
y
T
a
g
 
O
b
j
;
 
i
n
t
3
2
 
D
u
r
=
0
;
	
	
i
f
 
(
!
M
a
p
-
>
G
e
t
O
b
j
e
c
t
A
t
(
x
,
y
,
O
b
j
,
D
u
r
)
)
 
r
e
t
u
r
n
 
t
r
u
e
;
	
	
r
e
t
u
r
n
 
!
(
O
b
j
.
I
s
V
a
l
i
d
(
)
 
&
&
 
D
u
r
 
>
 
0
)
;
	
}
;
	
c
o
n
s
t
 
i
n
t
3
2
 
H
a
l
f
 
=
 
F
M
a
t
h
:
:
M
a
x
(
0
,
 
P
a
s
s
a
g
e
W
i
d
t
h
 
/
 
2
)
;
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
T
a
n
g
e
n
t
 
 
=
 
b
V
e
r
t
i
c
a
l
B
o
u
n
d
a
r
y
 
?
 
F
I
n
t
P
o
i
n
t
(
0
,
 
1
)
 
:
 
F
I
n
t
P
o
i
n
t
(
1
,
 
0
)
;
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
I
n
w
a
r
d
A
 
 
=
 
F
I
n
t
P
o
i
n
t
(
-
d
x
T
o
B
,
 
-
d
y
T
o
B
)
;
 
 
/
/
 
i
n
t
o
 
Z
o
n
e
 
A
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
O
u
t
w
a
r
d
B
 
=
 
F
I
n
t
P
o
i
n
t
(
+
d
x
T
o
B
,
 
+
d
y
T
o
B
)
;
 
 
/
/
 
i
n
t
o
 
Z
o
n
e
 
B
	
/
/
 
W
e
 
c
o
l
l
e
c
t
 
t
e
r
m
i
n
a
l
 
e
m
p
t
y
 
c
e
l
l
s
 
f
o
r
 
b
o
t
h
 
s
i
d
e
s
 
t
o
 
v
a
l
i
d
a
t
e
 
l
a
t
e
r
a
l
 
l
e
a
k
a
g
e
.
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
T
e
r
m
i
n
a
l
E
m
p
t
y
A
;
	
T
A
r
r
a
y
<
F
I
n
t
P
o
i
n
t
>
 
T
e
r
m
i
n
a
l
E
m
p
t
y
B
;
	
f
o
r
 
(
i
n
t
 
t
 
=
 
-
H
a
l
f
;
 
t
 
<
=
 
H
a
l
f
;
 
+
+
t
)
	
{
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
B
a
s
e
A
(
A
n
c
h
o
r
A
.
X
 
+
 
T
a
n
g
e
n
t
.
X
 
*
 
t
,
 
A
n
c
h
o
r
A
.
Y
 
+
 
T
a
n
g
e
n
t
.
Y
 
*
 
t
)
;
	
	
i
f
 
(
!
I
n
B
o
u
n
d
s
2
(
B
a
s
e
A
.
X
,
 
B
a
s
e
A
.
Y
)
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
	
/
/
 
-
-
-
 
A
 
s
i
d
e
:
 
w
a
l
k
 
i
n
s
i
d
e
 
Z
o
n
e
 
A
 
f
r
o
m
 
B
a
s
e
A
 
(
s
t
e
p
=
0
)
 
u
n
t
i
l
 
f
i
r
s
t
 
E
M
P
T
Y
 
A
 
c
e
l
l
.
	
	
{
	
	
	
b
o
o
l
 
b
S
t
o
p
p
e
d
 
=
 
f
a
l
s
e
;
	
	
	
i
n
t
 
s
t
e
p
 
=
 
0
;
	
	
	
w
h
i
l
e
 
(
t
r
u
e
)
	
	
	
{
	
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
p
(
B
a
s
e
A
.
X
 
+
 
I
n
w
a
r
d
A
.
X
 
*
 
s
t
e
p
,
 
B
a
s
e
A
.
Y
 
+
 
I
n
w
a
r
d
A
.
Y
 
*
 
s
t
e
p
)
;
	
	
	
	
i
f
 
(
!
I
n
B
o
u
n
d
s
2
(
p
.
X
,
 
p
.
Y
)
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
	
	
	
c
o
n
s
t
 
i
n
t
 
l
a
b
 
=
 
C
a
c
h
e
d
L
a
b
e
l
s
[
I
d
x
(
p
.
X
,
 
p
.
Y
,
 
W
)
]
;
	
	
	
	
i
f
 
(
l
a
b
 
!
=
 
Z
o
n
e
A
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
 
/
/
 
l
e
f
t
 
Z
o
n
e
 
A
 
b
e
f
o
r
e
 
h
i
t
t
i
n
g
 
a
n
 
e
m
p
t
y
 
A
 
c
e
l
l
	
	
	
	
i
f
 
(
I
s
O
c
c
u
p
i
e
d
(
M
a
p
,
 
p
.
X
,
 
p
.
Y
)
)
	
	
	
	
{
	
	
	
	
	
O
u
t
C
e
l
l
s
A
.
A
d
d
(
p
)
;
 
/
/
 
c
l
e
a
r
 
t
h
i
s
 
b
l
o
c
k
i
n
g
 
c
e
l
l
	
	
	
	
	
+
+
s
t
e
p
;
	
	
	
	
	
c
o
n
t
i
n
u
e
;
	
	
	
	
}
	
	
	
	
/
/
 
F
i
r
s
t
 
E
M
P
T
Y
 
A
-
c
e
l
l
 
r
e
a
c
h
e
d
 
?
 
r
e
m
e
m
b
e
r
 
f
o
r
 
l
e
a
k
a
g
e
 
c
h
e
c
k
 
a
n
d
 
s
t
o
p
 
t
h
i
s
 
c
o
l
u
m
n
 
o
n
 
A
 
s
i
d
e
	
	
	
	
T
e
r
m
i
n
a
l
E
m
p
t
y
A
.
A
d
d
(
p
)
;
	
	
	
	
b
S
t
o
p
p
e
d
 
=
 
t
r
u
e
;
	
	
	
	
b
r
e
a
k
;
	
	
	
}
	
	
	
i
f
 
(
!
b
S
t
o
p
p
e
d
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
	
}
	
	
/
/
 
-
-
-
 
B
 
s
i
d
e
:
 
s
t
a
r
t
 
a
t
 
s
t
e
p
=
1
 
(
f
i
r
s
t
 
Z
o
n
e
 
B
 
c
e
l
l
 
a
c
r
o
s
s
 
b
o
u
n
d
a
r
y
)
 
u
n
t
i
l
 
f
i
r
s
t
 
E
M
P
T
Y
 
B
 
c
e
l
l
.
	
	
{
	
	
	
b
o
o
l
 
b
S
t
o
p
p
e
d
 
=
 
f
a
l
s
e
;
	
	
	
i
n
t
 
s
t
e
p
 
=
 
1
;
 
/
/
 
I
M
P
O
R
T
A
N
T
:
 
s
t
a
r
t
 
a
c
r
o
s
s
 
t
h
e
 
b
o
u
n
d
a
r
y
	
	
	
w
h
i
l
e
 
(
t
r
u
e
)
	
	
	
{
	
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
p
(
B
a
s
e
A
.
X
 
+
 
O
u
t
w
a
r
d
B
.
X
 
*
 
s
t
e
p
,
 
B
a
s
e
A
.
Y
 
+
 
O
u
t
w
a
r
d
B
.
Y
 
*
 
s
t
e
p
)
;
	
	
	
	
i
f
 
(
!
I
n
B
o
u
n
d
s
2
(
p
.
X
,
 
p
.
Y
)
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
	
	
	
c
o
n
s
t
 
i
n
t
 
l
a
b
 
=
 
C
a
c
h
e
d
L
a
b
e
l
s
[
I
d
x
(
p
.
X
,
 
p
.
Y
,
 
W
)
]
;
	
	
	
	
i
f
 
(
l
a
b
 
!
=
 
Z
o
n
e
B
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
 
/
/
 
e
n
t
e
r
e
d
 
a
 
w
r
o
n
g
 
z
o
n
e
 
(
e
.
g
.
,
 
t
h
i
r
d
 
z
o
n
e
 
n
e
a
r
 
c
o
r
n
e
r
)
	
	
	
	
i
f
 
(
I
s
O
c
c
u
p
i
e
d
(
M
a
p
,
 
p
.
X
,
 
p
.
Y
)
)
	
	
	
	
{
	
	
	
	
	
O
u
t
C
e
l
l
s
B
.
A
d
d
(
p
)
;
 
/
/
 
c
l
e
a
r
 
u
n
t
i
l
 
w
e
 
r
e
a
c
h
 
e
m
p
t
y
 
B
	
	
	
	
	
+
+
s
t
e
p
;
	
	
	
	
	
c
o
n
t
i
n
u
e
;
	
	
	
	
}
	
	
	
	
/
/
 
F
i
r
s
t
 
E
M
P
T
Y
 
B
-
c
e
l
l
 
r
e
a
c
h
e
d
 
?
 
r
e
m
e
m
b
e
r
 
f
o
r
 
l
e
a
k
a
g
e
 
c
h
e
c
k
 
a
n
d
 
s
t
o
p
 
t
h
i
s
 
c
o
l
u
m
n
 
o
n
 
B
 
s
i
d
e
	
	
	
	
T
e
r
m
i
n
a
l
E
m
p
t
y
B
.
A
d
d
(
p
)
;
	
	
	
	
b
S
t
o
p
p
e
d
 
=
 
t
r
u
e
;
	
	
	
	
b
r
e
a
k
;
	
	
	
}
	
	
	
i
f
 
(
!
b
S
t
o
p
p
e
d
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
	
}
	
}
	
/
/
 
-
-
-
 
L
e
a
k
a
g
e
 
c
h
e
c
k
 
a
r
o
u
n
d
 
t
h
e
 
m
o
u
t
h
s
 
(
t
e
r
m
i
n
a
l
 
e
m
p
t
i
e
s
)
 
o
n
 
B
O
T
H
 
s
i
d
e
s
.
	
/
/
 
I
f
 
a
n
y
 
a
d
j
a
c
e
n
t
 
E
M
P
T
Y
 
c
e
l
l
 
b
e
l
o
n
g
s
 
t
o
 
a
 
t
h
i
r
d
 
z
o
n
e
 
(
n
o
t
 
A
/
B
)
,
 
r
e
j
e
c
t
 
t
h
i
s
 
a
n
c
h
o
r
.
	
a
u
t
o
 
F
a
i
l
s
L
e
a
k
C
h
e
c
k
 
=
 
[
&
]
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
M
o
u
t
h
)
-
>
b
o
o
l
	
{
	
	
s
t
a
t
i
c
 
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
N
4
[
4
]
 
=
 
{
 
{
1
,
0
}
,
{
-
1
,
0
}
,
{
0
,
1
}
,
{
0
,
-
1
}
 
}
;
	
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
o
 
:
 
N
4
)
	
	
{
	
	
	
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
 
q
(
M
o
u
t
h
.
X
 
+
 
o
.
X
,
 
M
o
u
t
h
.
Y
 
+
 
o
.
Y
)
;
	
	
	
i
f
 
(
!
I
n
B
o
u
n
d
s
2
(
q
.
X
,
 
q
.
Y
)
)
 
c
o
n
t
i
n
u
e
;
	
	
	
i
f
 
(
!
I
s
E
m
p
t
y
(
q
.
X
,
 
q
.
Y
)
)
 
c
o
n
t
i
n
u
e
;
 
/
/
 
b
l
o
c
k
e
d
:
 
f
i
n
e
	
	
	
c
o
n
s
t
 
i
n
t
 
l
a
b
 
=
 
C
a
c
h
e
d
L
a
b
e
l
s
[
I
d
x
(
q
.
X
,
 
q
.
Y
,
 
W
)
]
;
	
	
	
i
f
 
(
l
a
b
 
!
=
 
Z
o
n
e
A
 
&
&
 
l
a
b
 
!
=
 
Z
o
n
e
B
)
	
	
	
{
	
	
	
	
/
/
 
A
d
j
a
c
e
n
t
 
e
m
p
t
y
 
o
p
e
n
s
 
d
i
r
e
c
t
l
y
 
i
n
t
o
 
a
 
t
h
i
r
d
 
z
o
n
e
 
?
 
b
a
d
 
p
a
s
s
a
g
e
	
	
	
	
r
e
t
u
r
n
 
t
r
u
e
;
	
	
	
}
	
	
}
	
	
r
e
t
u
r
n
 
f
a
l
s
e
;
	
}
;
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
e
 
:
 
T
e
r
m
i
n
a
l
E
m
p
t
y
A
)
	
	
i
f
 
(
F
a
i
l
s
L
e
a
k
C
h
e
c
k
(
e
)
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
f
o
r
 
(
c
o
n
s
t
 
F
I
n
t
P
o
i
n
t
&
 
e
 
:
 
T
e
r
m
i
n
a
l
E
m
p
t
y
B
)
	
	
i
f
 
(
F
a
i
l
s
L
e
a
k
C
h
e
c
k
(
e
)
)
 
r
e
t
u
r
n
 
f
a
l
s
e
;
	
r
e
t
u
r
n
 
t
r
u
e
;
}
