(define (problem escape)
   (:domain escape)
   (:init
        (at_infirmary)
        (exit_locked)
   )

   (:goal 
        (and 
            (city)
        )
   )
)