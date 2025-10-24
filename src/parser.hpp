#pragma once
#include <string>
#include <map>
#include <vector>
#include <cstdint>
#include <nlohmann/json.hpp>
#include "parser/convert_flatmatrix.hpp"

namespace graphs {
namespace parser {

using GraphJsonRepresentation = std::map<uint64_t, std::vector<uint64_t>>;


template <typename Representation>
Representation representationFromJson(const std::string& data) {
  return nlohmann::json::parse(data).get<Representation>();
}

template <typename Representation>
std::string representationToJson(const Representation& data) {
  nlohmann::json j = data;
  return j.dump();
}


/**
 * Returns a graph from a JsonRepresentation coded in a std::string.
 * @param data The json representation of the graph, check documentation to see some examples.
 * @returns the Graph
 */
template <typename GraphT, typename GraphRepresentation = GraphJsonRepresentation>
GraphT fromjson(const std::string& data) {
  GraphT result;

  GraphRepresentation representation = (nlohmann::json(data)).get<GraphJsonRepresentation>();

  uint64_t maximumVertex = 0;
  for (auto& val : representation) maximumVertex = std::max(val.first, maximumVertex);

  result.addVertices(maximumVertex);

  for (auto& u : representation)
    for (auto v : u.second)
      result.addEdge(u.first, v);

  return result;
}

/**
 * Encodes the graph into a json.
 * @param graph The graph.
 * @returns The string with the encoded json of the graph.
 */
template <typename GraphT, typename GraphRepresentation = GraphJsonRepresentation>
std::string tojson(const GraphT& graph) {
  GraphRepresentation representation;

  for (uint64_t i = 0; i < graph.getVertexCount(); i++) {
    std::vector<uint64_t> edges = graph.getEdges(i);
    if (edges.size() != 0) representation[i] = std::move(edges);
  }

  nlohmann::json j = representation;
  return j.dump();
}

template <typename G>
G convert(const G& graph) {
  return graph;
}

template <typename Input, typename Output>
Output convert(const Input& input) {
  uint64_t N = input.getVertexCount();
  Output   output;
  output.addVertices(N);

  for (uint64_t u = 0; u < N; u++) {
    for (uint64_t v : input.getEdges(u))
      output.addEdge(u, v);
  }

  return output;
}


} // namespace parser
} // namespace graphs
