(define (problem monkey-problem)
   (:domain monkey-banana)
   (:objects A B C)
   (:init
    
     (at monkey C)
     (on-floor)
     (at box A)
     (at banana C)

   )

   (:goal 
       (and 
        (hasbanana)
       )
       )
)