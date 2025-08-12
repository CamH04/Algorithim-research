## Source Language Overview

The source language is a minimalist Lisp-style functional language consisting of:

* **Atoms**:

  * **Numbers** → encoded as Church numerals
  * **Symbols** → variables or function names
* **S-expressions**: `(operator arg1 arg2 ...)`

---

## Syntax

### Numbers

* Written as decimal integers (non-negative).
* Encoded as Church numerals (higher-order functions) in λ-calculus:

  ```
  0 → λf.λx.x
  1 → λf.λx.f x
  2 → λf.λx.f(f x)
  ```

### Variables

* Any non-numeric symbol.
* Bound by `lambda`, `let`, or function parameters.

### Function Definition

```
(lambda (x) (+ x 1))
```

Translates to:

```
λx.(ADD x 1)
```

### Function Application

```
(f a b)
```

Translates to:

```
((f a) b)
```

### Arithmetic

* Addition: `+`
* Multiplication: `*`

Example:

```
(+ 2 (* 3 4))
```

Translates to:

```
(ADD 2 (MUL 3 4))
```

Expanded with encodings:

```
(λm.λn.λf.λx. m f (n f x)) (λf.λx.f(f x)) ((λm.λn.λf. m (n f)) (λf.λx.f(f(f x))) (λf.λx.f(f(f(f x)))))
```

### Let Bindings

```
(let ((x 2) (y 3))
  (+ x y))
```

Translates to:

```
((λx.λy.(ADD x y)) 2 3)
```

---

## Compilation Pipeline

```
Source (.lctest)
   ↓ Parse (AST)
   ↓ Translate to λ-calculus IR
   ↓ Write .lc file
```

---

## Built-in Encodings

**Addition (ADD):**

```
λm.λn.λf.λx. m f (n f x)
```

**Multiplication (MUL):**

```
λm.λn.λf. m (n f)
```

**Booleans** *(planned)*:

```
TRUE  := λt.λf.t
FALSE := λt.λf.f
```

**If-Then-Else** *(planned)*:

```
IF := λb.λx.λy. b x y
```

---

## Usage

Compile:

```bash
make
```

Translate:

```
./lambdacalc example.lctest
```

This writes `example.lc` containing the λ-calculus IR.

---

## Example

`math.lctest`:

```
(let ((x 2))
  (+ x (* 3 4)))
```

Run:

```
./lambdacalc math.lctest
```

`math.lc` output:

```
((λx.(λm.λn.λf.λx. m f (n f x) x (λm.λn.λf. m (n f) (λf.λx.f(f(f x))) (λf.λx.f(f(f(f x))))))) (λf.λx.f(f x)))
```

---

Let me know if you want it formatted differently or expanded with extra details!
