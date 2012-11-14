<?php

/**
 * @file
 * Extended version of "views_handler_filter_term_node_tid" handler to process node count by term reference
 *
 */

class views_handler_filter_term_node_tid_with_node_count extends views_handler_filter_term_node_tid {

    /**
     * Extend default term reference exposed filter
     * @param $form
     * @param $form_state
     *
     */
    function value_form(&$form, &$form_state) {
        // Retrieve default form
        parent::value_form($form, $form_state);

        // Check if the nodes count is enable in this view
        if (isset($this->options['node_count']) && $this->options['node_count'] == 1) {

            // Check if exposed filter contains termes and node content types
            if (isset($form['value']['#options']) && isset($form_state['view']->filter['type']->value)) {
                $content_types = $form_state['view']->filter['type']->value;
                $terms_ids = array_keys($form['value']['#options']);

                // Retrieve node count by term reference
                $nodes_count = $this->_get_nodes_count_by_terms($terms_ids, $content_types);

                // Add node count after term name
                foreach ($form['value']['#options'] as $tid => $value) {
                    $node_count = (isset($nodes_count[$tid])) ? $nodes_count[$tid] : 0;
                    $form['value']['#options'][$tid] .= ' (' . $node_count . ')';
                }
            }
        }
    }

    /**
     * Retrieve node count by term reference
     * @param $terms_ids    array()
     * @param $content_types    array()
     * @return array retrieve node count by term reference
     */
    protected function _get_nodes_count_by_terms($terms_ids, $content_types) {

        global $language;
        $langcode = $language->language;
        $view_id = $this->view->vid;

        $data = &drupal_static(__FUNCTION__, NULL);

        if (!isset($data)) {
            $cache = cache_get("_get_nodes_count_by_terms:$langcode:$view_id");
            // Check if the cache is up to dat
            if (isset($cache->expire) && $cache->expire > REQUEST_TIME) {
                $data = $cache->data;
            }
            else {
                $data = array();

                // Select node count by terms in this views content type
                $query = db_select('taxonomy_index', 'ti');
                $query->join('node', 'n', 'ti.nid = n.nid');
                $query->addExpression('COUNT(*)', 'count');
                $query->fields('ti', array('tid'))
                    ->condition('n.status', 1)
                    ->condition('ti.tid', $terms_ids, 'IN')
                    ->condition('n.type', $content_types, 'IN')
                    ->groupBy('tid');


                $result = $query->execute();

                // Add node count for every term id
                foreach ($result as $item) {
                    $data[$item->tid] = $item->count;
                }

                // Set result in cache
                cache_set("_get_nodes_count_by_terms:$langcode:$view_id", $data, 'cache', time() + (60*60*2));
            }
        }
        return $data;
    }


    /**
     * Add option status for nodes count by terms
     * @return array
     */
    function option_definition() {
        $options = parent::option_definition();

        $options['node_count'] = array('default' => 0);

        return $options;
    }

    /**
     * Add checkbox status in exposed filter "parameter" form
     * @param $form
     * @param $form_state
     */
    function extra_options_form(&$form, &$form_state) {
        parent::extra_options_form($form, $form_state);

        $form['node_count'] = array(
            '#type' => 'checkbox',
            '#title' => t('Show nodes count by term reference'),
            '#default_value' => !empty($this->options['node_count']),
        );

    }


}