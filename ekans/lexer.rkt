#lang racket

(require "../common/common.rkt")

(provide lexer
         read-file)

(define (lexer input)
  (let ([input-list (if (string? input)
                        (string->list input)
                        input)])
    (if (null? input-list)
        (cons (cons 'eof '()) '())
        (let ([peek (car input-list)])
          (cond
            [(equal? peek lp) (cons (cons 'lparen '()) (cdr input-list))]
            [(equal? peek rp) (cons (cons 'rparen '()) (cdr input-list))]
            [else (cons (cons 'unknown peek) (cdr input-list))])))))

(define (read-file filename)
  (call-with-input-file filename
                        (lambda (port) (let ([content (port->string port)]) (string->list content)))))
