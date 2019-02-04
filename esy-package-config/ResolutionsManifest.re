type t = StringMap.t(resolution)

and resolution = {
  name: string,
  source,
  override: option(Json.t),
}

and source =
  | OfVersionSpec(VersionSpec.t)
  | OfLink(Link.t);

let empty = StringMap.empty;
let elements = StringMap.values;

module OfJson = {
  [@deriving of_yojson]
  type override = {
    source: SourceSpec.t,
    override: Json.t,
  };

  let of_yojson = json => {
    Result.Syntax.(
      {
        let f = (map, (name, json)) => {
          let%bind resolution =
            switch (json) {
            | `String(v) =>
              let%bind sourcespec = SourceSpec.parse(v);
              let source = OfVersionSpec(sourcespec);
              return({name, source, override: None});
            | `Assoc(_) =>
              let%bind override = override_of_yojson(json);
              let source = OfVersionSpec(override.source);
              return({name, source, override: Some(override.override)});
            | _ => errorf("expected a string or an object")
            };
          return(StringMap.add(name, resolution, map));
        };
        switch (json) {
        | `Assoc(fields) =>
          Result.List.foldLeft(~f, ~init=StringMap.empty, fields)
        | _ => error("expected an object")
        };
      }
    );
  };
};

let of_yojson = OfJson.of_yojson;
