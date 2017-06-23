(define (domain monkey-banana)
   (:requirements :adl)
   (:constants monkey box banana)
   (:predicates 
                 (goto ?y ?x) 
                 (climb ?x)
                 (push-box ?y ?x)
                 (grab ?y)
                 (on-floor)
                 (at ?y ?x)
                 (hasbanana)
                 (onbox ?x)
                 )

   (:action goto
             :parameters (?y ?x)
             :precondition (and 
                             (on-floor)
                             (at monkey ?y)
                             )
             :effect  (and 
                           (at monkey ?x)
                           (not (at monkey ?y))
                           )
    ) 


   (:action climb
             :parameters (?x)
             :precondition (and 
                             (at box ?x)
                             (at monkey ?x)
                             )
             :effect  (and 
                           (onbox ?x)
                           (not (on-floor))
                           )
    ) 


   (:action push-box
             :parameters (?y ?x)
             :precondition (and 
                             (at box ?y)
                             (at monkey ?y)
                             (on-floor)
                             )
             :effect  (and 
                           (at monkey ?x)
                           (at box ?x)
                           (not (at monkey ?y))
                           (not (at box ?y))
                           )
    ) 

   (:action grab
             :parameters (?y)
             :precondition (and 
                             (at banana ?y)
                             (onbox ?y)
                             )
             :effect  (and 
                           (hasbanana)
                           )
    ) 

)