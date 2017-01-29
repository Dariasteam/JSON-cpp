graph RL;
B(ObjectMap) -- components --> A(ObjectMap)
C(ObjectFinalNumberInt, 12) -- first --> B
D(ObjectFinalBool, true) -- second --> B
E(ObjectFinalString, 'this_is_a_string') -- third --> B

F(ObjectVector) -- factors --> A(ObjectMap)
G(ObjectFinalNumberFloat, 0,566) -- 0 --> F
H(ObjectFinalNumberBool, true) -- 1 --> F

