// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.

#pragma once

#include "NURBSGlobal.h"
#include "SingleCurve.h"
#include "BSplineBasis.h"

namespace NURBS
{

template <typename Real>
class BSplineCurve : public SingleCurve<Real>
{
public:
    // Construction and destruction.  The caller is responsible for deleting
    // the input arrays if they were dynamically allocated.  Internal copies
    // of the arrays are made, so to dynamically change control points or
    // knots you must use the 'SetControlPoint', 'GetControlPoint', and
    // 'Knot' member functions.

    // Uniform spline.  The number of control points is n+1 >= 2.  The degree
    // of the B-spline is d and must satisfy 1 <= d <= n.  The knots are
    // implicitly calculated in [0,1].  If open is 'true', the spline is
    // open and the knots are
    //   t[i] = 0,               0 <= i <= d
    //          (i-d)/(n+1-d),   d+1 <= i <= n
    //          1,               n+1 <= i <= n+d+1
    // If open is 'false', the spline is periodic and the knots are
    //   t[i] = (i-d)/(n+1-d),   0 <= i <= n+d+1
    // If loop is 'true', extra control points are added to generate a closed
    // curve.  For an open spline, the control point array is reallocated and
    // one extra control point is added, set to the first control point
    // C[n+1] = C[0].  For a periodic spline, the control point array is
    // reallocated and the first d points are replicated.  In either case the
    // knot array is calculated accordingly.

    BSplineCurve (int numCtrlPoints, const Array1D_Vector3& ctrlPoint,
        int degree, bool loop, bool open);

    // Open, nonuniform spline.  The knot array must have n-d elements.  The
    // elements must be nondecreasing.  Each element must be in [0,1].
    BSplineCurve (int numCtrlPoints, const Array1D_Vector3& ctrlPoint,
        int degree, bool loop, const Real* knot);

    int GetNumCtrlPoints () const;
    int GetDegree () const;
    bool IsOpen () const;
    bool IsUniform () const;
    bool IsLoop () const;

    // Control points may be changed at any time.  The input index should be
    // valid (0 <= i <= n).  If it is invalid, GetControlPoint returns a
    // vector whose components are all MAX_REAL.
    void SetControlPoint (int i, const Vector3& ctrl);
    Vector3 GetControlPoint (int i) const;

    // The knot values can be changed only if the basis function is nonuniform
    // and the input index is valid (0 <= i <= n-d-1).  If these conditions
    // are not satisfied, GetKnot returns MAX_REAL.
    void SetKnot (int i, Real knot);
    Real GetKnot (int i) const;
	Array1D_Real GetKnotVector(bool isInnerOnly = true);

    // The spline is defined for 0 <= t <= 1.  If a t-value is outside [0,1],
    // an open spline clamps t to [0,1].  That is, if t > 1, t is set to 1;
    // if t < 0, t is set to 0.  A periodic spline wraps to to [0,1].  That
    // is, if t is outside [0,1], then t is set to t-floor(t).
    virtual Vector3 GetPosition (Real t) ;
    virtual Vector3 GetFirstDerivative (Real t) ;
    virtual Vector3 GetSecondDerivative (Real t) ;
    virtual Vector3 GetThirdDerivative (Real t) ;

    // If you need position and derivatives at the same time, it is more
    // efficient to call these functions.  Pass the addresses of those
    // quantities whose values you want.  You may pass 0 in any argument
    // whose value you do not want.
    void Get (Real t, Vector3* pos, Vector3* der1,
        Vector3* der2, Vector3* der3);

    // Access the basis function to compute it without control points.  This
    // is useful for least squares fitting of curves.
    BSplineBasis<Real>& GetBasis ();

protected:
    // Replicate the necessary number of control points when the Create
    // function has loop equal to true, in which case the spline curve must
    // be a closed curve.
    void CreateControl (const Array1D_Vector3& ctrlPoint);

    int mNumCtrlPoints;
    Array1D_Vector3 mCtrlPoint;  // ctrl[n+1]
    bool mLoop;
    BSplineBasis<Real> mBasis;
    int mReplicate;  // the number of replicated control points
};

typedef BSplineCurve<float> BSplineCurvef;
typedef BSplineCurve<double> BSplineCurved;

}

