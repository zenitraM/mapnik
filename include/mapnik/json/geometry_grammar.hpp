/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2012 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef MAPNIK_GEOMETRY_GRAMMAR_HPP
#define MAPNIK_GEOMETRY_GRAMMAR_HPP

// mapnik
#include <mapnik/geometry.hpp>  // for geometry_type
#include <mapnik/vertex.hpp>  // for CommandType

// spirit::qi
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_function.hpp>

namespace mapnik { namespace json {

namespace qi = boost::spirit::qi;
namespace standard_wide =  boost::spirit::standard_wide;
using standard_wide::space_type;

struct push_vertex
{
    typedef void result_type;

    template <typename T0,typename T1, typename T2, typename T3>
    result_type operator() (T0 c, T1 path, T2 x, T3 y) const
    {
        BOOST_ASSERT( path!=0 );
        path->push_vertex(x,y,c);
    }
};

struct close_path
{
    typedef void result_type;

    template <typename T>
    result_type operator() (T path) const
    {
        BOOST_ASSERT( path!=0 );
        if (path->size() > 2u) // to form a polygon ring we need at least 3 vertices
        {
            path->close_path();
        }
    }
};

struct cleanup
{
    typedef void result_type;
    template <typename T0>
    void operator() (T0 & path) const
    {
        if (path) delete path, path=0;
    }
};

struct where_message
{
    typedef std::string result_type;

    template <typename Iterator>
    std::string operator() (Iterator first, Iterator last, std::size_t size) const
    {
        std::string str(first, last);
        if (str.length() > size)
            return str.substr(0, size) + "..." ;
        return str;
    }
};

template <typename Iterator>
struct geometry_grammar :
        qi::grammar<Iterator,qi::locals<int>, void(boost::ptr_vector<mapnik::geometry_type>& )
        , space_type>
{
    geometry_grammar();
    qi::rule<Iterator, qi::locals<int>, void(boost::ptr_vector<mapnik::geometry_type>& ),space_type> geometry;
    qi::symbols<char, int> geometry_dispatch;

    qi::rule<Iterator,void(CommandType,geometry_type*),space_type> point;
    qi::rule<Iterator,qi::locals<CommandType>,void(geometry_type*),space_type> points;
    qi::rule<Iterator,void(boost::ptr_vector<mapnik::geometry_type>&,int),space_type> coordinates;
    //
    qi::rule<Iterator,qi::locals<geometry_type*>,
             void(boost::ptr_vector<mapnik::geometry_type>& ),space_type> point_coordinates;
    qi::rule<Iterator,qi::locals<geometry_type*>,
             void(boost::ptr_vector<mapnik::geometry_type>& ),space_type> linestring_coordinates;
    qi::rule<Iterator,qi::locals<geometry_type*>,
             void(boost::ptr_vector<mapnik::geometry_type>& ),space_type> polygon_coordinates;

    qi::rule<Iterator,void(boost::ptr_vector<mapnik::geometry_type>& ),space_type> multipoint_coordinates;
    qi::rule<Iterator,void(boost::ptr_vector<mapnik::geometry_type>& ),space_type> multilinestring_coordinates;
    qi::rule<Iterator,void(boost::ptr_vector<mapnik::geometry_type>& ),space_type> multipolygon_coordinates;
    qi::rule<Iterator,void(boost::ptr_vector<mapnik::geometry_type>& ),space_type> geometry_collection;

    // Nabialek trick //////////////////////////////////////
    //typedef typename qi::rule<Iterator,void(FeatureType &), space_type> dispatch_rule;
    //qi::rule<Iterator,qi::locals<dispatch_rule*>, void(FeatureType&),space_type> geometry;
    //qi::symbols<char, dispatch_rule*> geometry_dispatch;
    ////////////////////////////////////////////////////////

    boost::phoenix::function<push_vertex> push_vertex_;
    boost::phoenix::function<close_path> close_path_;
    boost::phoenix::function<cleanup> cleanup_;
    boost::phoenix::function<where_message> where_message_;
};

}}

#endif // MAPNIK_GEOMETRY_GRAMMAR_HPP
