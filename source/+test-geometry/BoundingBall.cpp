#include "Test.h"
#include "BoundingBall.h"

TEST_CASE(BoundingBall) {
    //Log::Scope scope (Log::TRACE);
    Vector2 points[] = {Vector2 (0, 0), Vector2 (1, 0), Vector2 (0, 1)};
    auto ball = BoundingBall2::fromPoints (points, 3);

    TEST_EQUALS(ball.getRadius(), sqrt (2) / 2);
    TEST_FALSE(ball.isEmpty());
    TEST_TRUE(ball.contains(Vector2(0, 0)));
    TEST_TRUE(ball.contains(Vector2(1, 0)));
    TEST_TRUE(ball.contains(Vector2(0, 1)));
    TEST_TRUE(ball.contains(Vector2(0.25, 0.25)));
    TEST_TRUE(ball.contains(Vector2(0.75, 0.75)));
    TEST_FALSE(ball.contains(Vector2(0, 2)));
    TEST_FALSE(ball.contains(Vector2(2, 0)));
    TEST_FALSE(ball.contains(Vector2(0, -2)));
    TEST_FALSE(ball.contains(Vector2(-2, 0)));
    TEST_EQUALS(ball.getCenter(), Vector2(0.5, 0.5));
}

TEST_CASE(From2BoundaryPoints) {
    //Log::Scope scope (Log::TRACE);
    BoundingBall2::PointList points = { Vector2 (1, 0), Vector2 (-1, 0) };
    Log::debug () << "Points size: " << points.size () << endl;
    auto ball = BoundingBall2::makeBall (points);
    Log::debug () << "center: " << ball.getCenter () << endl;
    TEST_FALSE(ball.isEmpty());
    TEST_EQUALS(ball.getRadius(), 1);
}

TEST_CASE(From3BoundaryPoints) {
    //Log::Scope scope (Log::TRACE);
    BoundingBall2::PointList points = { Vector2 (-3, 4), Vector2 (4, 5), Vector2(1, -4) };
    Log::debug () << "Points size: " << points.size () << endl;
    auto ball = BoundingBall2::makeBall (points);
    Log::debug () << "center: " << ball.getCenter () << endl;
    TEST_FALSE(ball.isEmpty());
    TEST_EQUALS(ball.getCenter(), Vector2 (1, 1));
    TEST_EQUALS(ball.getRadius(), 5);
}

TEST_CASE(From3BoundaryPointsRandom) {
    //Log::Scope scope (Log::TRACE);

    random_device randomDevice;
    mt19937 twister (randomDevice ());
    std::uniform_real_distribution<f8> dist(1.0, nextafter(10.0, numeric_limits<f8>::max ()));

    for (int i = 0; i < 100; ++i) {
        // generate a random circle
        f8 radius = dist (twister);
        Point2 center (dist (twister), dist (twister));

        // generate three points on the circle at random
        f8 alpha;
        alpha = ((dist (twister) - 1.0) / 9.0) * M_PI * 2;
        Point2 a = (Point2 (cos (alpha), sin (alpha)) * radius) + center;
        alpha = ((dist (twister) - 1.0) / 9.0) * M_PI * 2;
        Point2 b = (Point2 (cos (alpha), sin (alpha)) * radius) + center;
        alpha = ((dist (twister) - 1.0) / 9.0) * M_PI * 2;
        Point2 c = (Point2 (cos (alpha), sin (alpha)) * radius) + center;

        // now compute the bounding ball
        BoundingBall2::PointList points = {a, b, c };
        auto ball = BoundingBall2::makeBall (points);

        // and confirm our result matches the setup
        TEST_FALSE(ball.isEmpty());
        TEST_EQUALS(ball.getCenter(), center);
        TEST_EQUALS_EPS(ball.getRadius(), radius, Point2::getEpsilon ());
    }
}

TEST_CASE(Algorithm1) {
    Log::Scope scope (Log::TRACE);
    Log::debug () << "Algorithm1" << endl;

    random_device randomDevice;
    mt19937 twister (randomDevice ());
    std::uniform_real_distribution<f8> dist(0.0, nextafter(1.0, numeric_limits<f8>::max ()));

    for (int i = 0; i < 100; ++i) {
        // generate a random circle
        Point2 center (dist (twister), dist (twister));
        f8 radius = dist (twister);

        // generate three points on the circle at random, these are a forced boundary - there is no
        // reason a circle bigger than this should be generated by the fitter
        f8 alpha;
        alpha = dist (twister) * M_PI * 2;
        Point2 a = (Point2 (cos (alpha), sin (alpha)) * radius) + center;
        alpha = dist (twister) * M_PI * 2;
        Point2 b = (Point2 (cos (alpha), sin (alpha)) * radius) + center;
        alpha = dist (twister) * M_PI * 2;
        Point2 c = (Point2 (cos (alpha), sin (alpha)) * radius) + center;
        BoundingBall2::PointList points = {a, b, c};

        // make a ball from these three points - to be sure we aren't dealing with numerical error
        auto generator = BoundingBall2::makeBall(points);
        if (generator.getRadius () >= 0) {
            Log::debug () << "Generator: " << generator << endl;

            // fill the circle with a bunch of random points - not too many, if we generate a lot,
            // then the bound will tend to be defined uniformly - we'd rather have a lot of
            // variance in the circle that needs to be matched to compare results meaningfully
            for (int j = 0; j < 7; ++j) {
                alpha = dist (twister) * M_PI * 2;
                Point2 d = (Vector2 (cos (alpha), sin (alpha)) * (generator.getRadius() * (dist (twister) * 0.99))) + generator.getCenter();
                points.push_front (d);
            }

            // compute the bounding ball
            BoundingBall2::PointList boundaryPoints;
            auto ball = BoundingBall2::algorithmMoveToFront (points, points.end(), boundaryPoints);

            // and confirm our result is the same or better than the setup
            TEST_FALSE(ball.isEmpty());
            TEST_XYOP(ball.getRadius(), generator.getRadius() + Point2::getEpsilon(), <=);

            // run through all the points and be sure they are contained
            Log::debug () << ball << endl;
            Log::debug () << "a: " << a << ", b: " << b << ", c: " << c << endl;
            Log::debug () << "Points (in important order):" << endl;
            for (BoundingBall2::PointListIterator iter = points.begin (); iter != points.end (); ++iter) {
                Log::debug () << "  Point: " << *iter << ", Delta: " << (*iter - ball.getCenter ()).length () << endl;
                TEST_TRUE (ball.contains(*iter));
            }

            // compare the center to one generated from a bounding box approach
            vector<Point2> ptsvec (points.begin (), points.end ());
            auto boxBall = BoundingBall2::fromPoints(ptsvec.data(), ptsvec.size());
            Log::debug () << "Compared to bounding box approach, delta center: " <<
                    (ball.getCenter () - boxBall.getCenter ()).length () <<
                    ", delta radius: " << (100 * (ball.getRadius () - boxBall.getRadius ()) / boxBall.getRadius ()) <<
                    "%" << endl;

            Log::debug () << endl;
        } else {
            Log::debug () << "Skipping bad ball" << endl;
        }
    }
}
