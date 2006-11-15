// Copyright (c) 2003,2004  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Menelaos Karavelas <mkaravel@cse.nd.edu>



#ifndef CGAL_PARABOLA_2_H
#define CGAL_PARABOLA_2_H

#include <CGAL/determinant.h>


CGAL_BEGIN_NAMESPACE


template < class Gt >
class Parabola_2
{
private:
  typedef Parabola_2<Gt>  Self;
public:
  typedef typename Gt::Site_2                 Site_2;
  typedef typename Gt::Point_2                Point_2;
  typedef typename Gt::Segment_2              Segment_2;
  typedef typename Gt::Line_2                 Line_2;
  typedef typename Gt::FT                     FT;
  //  typedef CGAL::Point_2< Cartesian<double> >      Point_2;
  //  typedef CGAL::Segment_2< Cartesian<double> >    Segment_2;
  //  typedef CGAL::Line_2< Cartesian<double> >       Line_2;

protected:
  // static stuff
#if defined(__POWERPC__) && \
  defined(__GNUC__) && (__GNUC__ == 3) && (__GNUC_MINOR__ == 4)
  // hack to avoid nasty warning for G++ 3.4 on Darwin
  static FT STEP()
  {
    return FT(2);
  }
#else
  static const FT& STEP()
  {
    static FT step_(2);
    return step_;
  }
#endif

  //  inline static
  //  FT square(const FT &x)
  //  {
  //    return x * x;
  //  }

  inline static
  FT divide(const FT& x, const FT& y, const Tag_false&) {
    return CGAL::to_double(x) / CGAL::to_double(y);
  }

  inline static
  FT divide(const FT& x, const FT& y, const Tag_true&) {
    return x / y;
  }

  inline static
  FT divide(const FT& x, const FT& y) {
      return divide(x, y, 
      Boolean_tag< ! CGALi::Is_integral_domain_without_division<FT>::value >());
  }
    
  inline static
  FT sqrt(const FT& x, const Tag_false&) {
    return CGAL::sqrt(CGAL::to_double(x));
  }

  inline static
  FT sqrt(const FT& x, const Tag_true&) {
    return CGAL::sqrt(x);
  }

  inline static
  FT sqrt(const FT& x) {
      return sqrt(x, Boolean_tag<CGALi::Is_field_with_sqrt<FT>::value >());
  }

  inline static
  FT norm2(const Point_2& p)
  {
    return CGAL::square(p.x()) + CGAL::square(p.y());
  }

  inline static
  FT distance2(const Point_2& p1, const Point_2& p2)
  {
    FT dx = p1.x()-p2.x();
    FT dy = p1.y()-p2.y();
    return CGAL::square(dx) + CGAL::square(dy);
  }

  inline static
  FT distance(const Point_2& p1, const Point_2& p2)
  {
    return sqrt( distance2(p1, p2) );
  }

  inline static
  FT distance(const Point_2& p, const Line_2& l)
  {
    return divide( p.x() * l.a() + p.y() * l.b() + l.c(),
		   sqrt( CGAL::square(l.a()) + CGAL::square(l.b()) ) );
  }

  // instance stuff
  Point_2 c;
  Line_2 l;
  Point_2 o;

  inline
  Point_2 lchain(const FT &t) const
  {
    std::vector< Point_2 > p = compute_points(t);
    if ( right(p[0]) )  return p[1];
    return p[0];
  }

  inline
  Point_2 rchain(const FT &t) const
  {
    std::vector< Point_2 > p = compute_points(t);
    if ( right(p[0]) )  return p[0];
    return p[1]; 
  }

  std::vector< Point_2 > compute_points(const FT &d) const
  {
    assert(d >= 0);
    FT d1 = distance(o, c) + d;
    FT d2 = distance(o, l) + d;
    d2 = d1;
    d1 *= d1;

    std::vector< Point_2 > p;

    if ( l.a() == ZERO ) {
      FT y = d2 * CGAL::sign(l.b()) - divide(l.c(), l.b());

      FT C = CGAL::square(y) - FT(2) * c.y() * y + 
	CGAL::square(c.x()) + CGAL::square(c.y()) - d1;

      FT D = CGAL::square(c.x()) - C;

      D = CGAL::abs(D);

      FT x1 = sqrt(D) + c.x();
      FT x2 = -sqrt(D) + c.x();

      p.push_back(Point_2(x1, y));
      p.push_back(Point_2(x2, y));

      return p;
    }

    FT A = d2 * sqrt( CGAL::square(l.a()) + CGAL::square(l.b()) ) - l.c();
    FT B = CGAL::square(c.x()) + CGAL::square(c.y()) - d1;

    FT alpha = FT(1) + CGAL::square(divide(l.b(), l.a()));
    FT beta = divide(A * l.b(), CGAL::square(l.a())) + c.y()
      - divide(c.x() * l.b(), l.a());
    FT gamma = CGAL::square(divide(A, l.a())) + B
      - divide(FT(2) * c.x() * A, l.a());

    FT D = CGAL::square(beta) - alpha * gamma;

    D = CGAL::abs(D);

    FT y1 = divide((beta + sqrt(D)), alpha);
    FT y2 = divide((beta - sqrt(D)), alpha);

    FT x1 = divide(A - l.b() * y1, l.a());
    FT x2 = divide(A - l.b() * y2, l.a());

    p.push_back(Point_2(x1, y1));
    p.push_back(Point_2(x2, y2));

    return p;
  }

  bool right(const Point_2& p) const
  {
    return
      CGAL::is_positive( det3x3_by_formula<FT>(c.x(), c.y(), FT(1),
					       o.x(), o.y(), FT(1),
					       p.x(), p.y(), FT(1)) );
  }

  inline
  Point_2 midpoint(const Point_2& p1, const Point_2& p2) const
  {
    FT t1 = t(p1);
    FT t2 = t(p2);
    FT midt = divide(t1+t2, FT(2));
    return f(midt);
  }

  inline
  Point_2 f(FT t) const
  {
    if ( CGAL::is_negative(t) )  return rchain(-t);
    return lchain(t);
  }

  inline
  FT t(const Point_2 &p) const
  {
    FT tt = distance(p, c) - distance(c, o);
    if ( right(p) )  return -tt;
    return tt;
  }

  void compute_origin()
  {
    FT d = divide(l.a() * c.x() + l.b() * c.y() + l.c(),
		  FT(2) * ( CGAL::square(l.a()) + CGAL::square(l.b()) )  );
    o = Point_2(c.x() - l.a() * d, c.y() - l.b() * d);
  }

public:
  Parabola_2() {}

  template<class ApolloniusSite>
  Parabola_2(const ApolloniusSite &p, const Line_2 &l1)
  {
    this->c = p.point();

    FT d_a = CGAL::to_double(l1.a());
    FT d_b = CGAL::to_double(l1.b());
    FT len = sqrt(CGAL::square(d_a) + CGAL::square(d_b));

    FT r = p.weight() * len;

    this->l = Line_2(-l1.a(), -l1.b(), -l1.c() + r);
    compute_origin();
  }

  Parabola_2(const Point_2 &p, const Line_2 &line)
  {
    this->c = p;

    if ( line.has_on_positive_side(p) ) {
      this->l = line;
    } else {
      this->l = line.opposite();
    }
    compute_origin();
  }


  Oriented_side
  side_of_parabola(const Point_2& p) const
  {
    Point_2 q(CGAL::to_double(p.x()), CGAL::to_double(p.y()));

    FT d = distance(q, c) - fabs(distance(q, l));
    if ( d < 0 )  return ON_NEGATIVE_SIDE;
    if ( d > 0 )  return ON_POSITIVE_SIDE;
    return ON_ORIENTED_BOUNDARY;
  }


  inline Line_2 line() const
  {
    return l;
  }

  inline Point_2 center() const
  {
    return c;
  }

  template< class Stream >
  void draw(Stream& W) const
  {
    std::vector< Point_2 > p;
    std::vector< Point_2 > pleft, pright;

    pleft.push_back(o);
    pright.push_back(o);

    for (int i = 1; i <= 100; i++) {
      p = compute_points(i * i * STEP());

      W << p[0];
      W << p[1];

      if ( p.size() > 0 ) {
	if ( right(p[0]) ) {
	  pright.push_back(p[0]);
	  pleft.push_back(p[1]);
	} else {
	  pright.push_back(p[1]);
	  pleft.push_back(p[0]);
	}
      }
    }

    for (unsigned int i = 0; i < pleft.size() - 1; i++) {
      W << Segment_2(pleft[i], pleft[i+1]);
    }

    for (unsigned int i = 0; i < pright.size() - 1; i++) {
      W << Segment_2(pright[i], pright[i+1]);
    }

    W << o;
  }
};

template< class Stream, class Gt >
inline
Stream& operator<<(Stream& s, const Parabola_2<Gt> &P)
{
  P.draw(s);
  return s;
}




CGAL_END_NAMESPACE

#endif // CGAL_PARABOLA_2_H
