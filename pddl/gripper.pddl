(define (domain gripper)
	(:requirements :strips)

	(:predicates
		(ROOM ?x) (BALL ?x) (GRIPPER ?x)
		(at-robby ?x) (at-ball ?x ?y)
		(free ?x) (carry ?x ?y))

	(:action move
		:parameters (?x ?y)
		:precondition 
                               (and (ROOM ?x) (ROOM ?y) (not (eq ?x ?y)) (at-robby ?x))
		:effect 
                               (and (at-robby ?y) (not (at-robby ?x)))
        )


	(:action move1
		:parameters (?x ?y)
		:precondition 
                               (and (ROOM ?x) (ROOM ?y) (not (eq ?x ?y)) (at-robby ?x))
		:effect 
                               (and (at-robby ?y) (not (at-robby ?x)))
        )

)
